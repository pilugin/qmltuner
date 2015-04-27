#ifndef QMLTUNERSERVER_H
#define QMLTUNERSERVER_H

#include <QObject>

class QApplication;
class QmlItemManager;

class QmlTunerServer : public QObject
{
public:
    static void init(QApplication *app);
private:
    QmlTunerServer(QApplication *app);

    QmlItemManager *mQmlItemMgr;

    static bool sActivated;
};

#endif // QMLTUNERSERVER_H
