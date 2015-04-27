#ifndef QMLTUNERSERVER_H
#define QMLTUNERSERVER_H

#include <QObject>

class QGuiApplication;
class QmlItemManager;

class QmlTunerServer : public QObject
{
public:
    static void init(QGuiApplication *app);
private:
    QmlTunerServer(QGuiApplication *app);

    QmlItemManager *mQmlItemMgr;

    static bool sActivated;
};

#endif // QMLTUNERSERVER_H
