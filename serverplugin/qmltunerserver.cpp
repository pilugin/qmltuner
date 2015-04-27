#include "qmltunerserver.h"
#include <QtGui/QGuiApplication>
#include <QtDBus/QDBusConnection>
#include <QtDebug>

#include "qmlitemmanager.h"

bool QmlTunerServer::sActivated = false;

void QmlTunerServer::init(QGuiApplication *app)
{
    Q_ASSERT(app);

    static QmlTunerServer instance(app);
    Q_UNUSED(instance);
}

QmlTunerServer::QmlTunerServer(QGuiApplication *) : mQmlItemMgr(0)
{
    const QByteArray envStr = qgetenv("QML_TUNER");

    bool on = envStr.toInt() != 0;
    if (!on) {
        if (!envStr.isEmpty()) {
            QList<QByteArray> paramList(envStr.split(':'));
            if (paramList.size()==2 && paramList.first()=="app")
                on = (paramList.last() == qApp->arguments().first().split("/").last());
        }
    }

    if (on) {
        qDebug()<<"[QMLTUNER] initializing...  ";

        mQmlItemMgr = new QmlItemManager(this);
        sActivated = QDBusConnection::sessionBus().registerService("org.pilugin.QmlTuner");
        sActivated = sActivated && QDBusConnection::sessionBus().registerObject("/qmltuner", this);
        if (sActivated)
            qDebug() << "[QMLTUNER] Ready";
        else
            qDebug() << "[QMLTUNER] Failed to init";
    }
}
