#include "form.h"
#include "ui_form.h"
#include <QInputDialog>
#include <QFileDialog>

static const char qml_tuner_service[] = "com.luxoft.qmltuner";
static const char qml_tuner_object[] = "/qmltuner";

Form::Form(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Form),
      mSettings(QLatin1String("luxoft"), QLatin1String("qmltuner"))
{
    ui->setupUi(this);
    ui->itemsView->setModel( &mItemsModel );
    ui->propertiesView->setModel( &mPropertiesModel );

    mQmlTunerProxy = new com::luxoft::qmltuner(QString::fromLatin1(qml_tuner_service),
                                               QString::fromLatin1(qml_tuner_object),
                                               QDBusConnection::sessionBus(), this);

    mServiceWatcher.setConnection(QDBusConnection::sessionBus());
    mServiceWatcher.addWatchedService(QString::fromLatin1(qml_tuner_service));

    connect(&mServiceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(onServiceUnregistered(QString)));
    connect(&mServiceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onServiceRegistered(QString)));
}

Form::~Form()
{
    delete ui;
}

void Form::on_refreshItems_clicked()
{
    if (!mQmlTunerProxy)
        return;

    QDBusPendingReply< QVector<ItemNotation> > reply = mQmlTunerProxy->enumerateItems();
    reply.waitForFinished();

    if (reply.isError()) {
        qDebug()<<"ERROR"<<reply.error().name()<<reply.error().message();
    } else {
        mItemsModel.updateInfo(reply.value());
    }

    mPropertiesModel.clear();
}

void Form::on_itemsView_clicked(const QModelIndex &index)
{
    if (!mQmlTunerProxy)
        return;

    ItemNotation itemNotation(mItemsModel.itemNotation(index));
    mPropertiesModel.updateInfo( itemNotation );
    mQmlTunerProxy->highlight(itemNotation.address);

    mItemAddress = itemNotation.address;
}

void Form::on_propertiesView_doubleClicked(const QModelIndex &index)
{
    if (!mQmlTunerProxy)
        return;

    if (index.column() == 1) {
        QString prop = mPropertiesModel.index(index.row(), 0, QModelIndex()).data(Qt::DisplayRole).toString();
        QString value = index.data(Qt::DisplayRole).toString();

        bool ok = false;
        QString str = QInputDialog::getText(this, tr("Edit"), prop,
                                            QLineEdit::Normal, value, &ok);
        if (ok) {
            QDBusPendingReply<bool> reply = mQmlTunerProxy->setProperty(mItemAddress, prop, str);
            reply.waitForFinished();

            if (reply.isError()) {
                qDebug()<<"ERROR"<<reply.error().name()<<reply.error().message();
                return;
            }

            if (!reply.value()) {
                qDebug()<<"FAILED TO SET PROPERTY";

            } else {
                QDBusPendingReply<ItemNotation> reply = mQmlTunerProxy->describeItem(mItemAddress);
                reply.waitForFinished();
                if (reply.isError()) {
                    qDebug()<<"ERROR"<<reply.error().name()<<reply.error().message();
                    return;
                }


                if (!reply.value().valid())
                    qDebug()<<"ITEMNOTATION INVALID";
                mPropertiesModel.propUpdated(mItemAddress, prop);
                mItemsModel.itemUpdated(mItemAddress);

                mPropertiesModel.updateInfo(reply.value());

            }
        }
    }
}

void Form::on_showTarget_clicked()
{
    if (!mQmlTunerProxy)
        return;

    if (mTargetPathToFile.isEmpty())
        mTargetPathToFile = mSettings.value("prevTarget").toString();

    QString tmp = QFileDialog::getOpenFileName(this, "Choose image file", mTargetPathToFile, "*png");
    if (!tmp.isEmpty()) {
        QDBusPendingReply<bool> reply = mQmlTunerProxy->showTarget(tmp, opacityValueConvert(ui->opacity->value()));
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug()<<"ERROR"<<reply.error().name()<<reply.error().message();
            return;
        }

        if (!reply.value()) {
            qDebug()<<"FAILED TO LOAD TARGET";
        } else {
            mTargetPathToFile = tmp;
            mSettings.setValue("prevTarget", mTargetPathToFile);
        }
    }
}

void Form::on_opacity_valueChanged(int value)
{
    if (!mQmlTunerProxy)
        return;

    mQmlTunerProxy->showTarget(mTargetPathToFile, opacityValueConvert(value));
}

void Form::onServiceRegistered(const QString &service)
{
    qDebug()<<__FUNCTION__<<service;
    if (service == QString::fromLatin1(qml_tuner_service)) {
        if (!mQmlTunerProxy) {
            mQmlTunerProxy = new com::luxoft::qmltuner(QString::fromLatin1(qml_tuner_service),
                                                       QString::fromLatin1(qml_tuner_object),
                                                       QDBusConnection::sessionBus(), this);
        }

        on_refreshItems_clicked();
    }
}

void Form::onServiceUnregistered(const QString &service)
{
    qDebug()<<__FUNCTION__<<service;
    if (service == QString::fromLatin1(qml_tuner_service)) {
        if (mQmlTunerProxy) {
            mQmlTunerProxy->deleteLater();
            mQmlTunerProxy = 0;
        }
    }
}

double Form::opacityValueConvert(int value) const
{
    double distance = ui->opacity->maximum() - ui->opacity->minimum();
    double res = (double(ui->opacity->minimum()) + double(value)) / distance;

    return res;
}
