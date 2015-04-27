#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "qmlitemstree.h"
#include "itempropertiesmodel.h"
#include "qmltunerproxy.h"
#include <QSettings>
#include <QDBusServiceWatcher>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private Q_SLOTS:
    void on_refreshItems_clicked();

    void on_itemsView_clicked(const QModelIndex &index);

    void on_propertiesView_doubleClicked(const QModelIndex &index);

    void on_showTarget_clicked();

    void on_opacity_valueChanged(int value);

    void onServiceRegistered(const QString &service);
    void onServiceUnregistered(const QString &service);

private:
    Ui::Form *const ui;

    QmlItemsTree mItemsModel;
    ItemPropertiesModel mPropertiesModel;

    QString mItemAddress;
    QString mTargetPathToFile;

    com::luxoft::qmltuner *mQmlTunerProxy;
    QDBusServiceWatcher mServiceWatcher;

    double opacityValueConvert(int value) const;

    QSettings mSettings;
};

#endif // FORM_H
