#include "qmltunerplugin.h"
#include "qmltunerserver.h"
#include <QtQml>
#include <QtGui>
#include <QtDebug>

class QmlTunerDummy : public QObject
{

};

QmlTunerPlugin::QmlTunerPlugin()
{
    qDebug()<<"[QMLTUNER] QmlTunerPlugin::QmlTunerPlugin()";
}

QmlTunerPlugin::~QmlTunerPlugin()
{
    qDebug()<<"[QMLTUNER] QmlTunerPlugin::~QmlTunerPlugin()";
}

void QmlTunerPlugin::registerTypes(const char *uri)
{
    qDebug()<<"[QMLTUNER] QmlTunerPlugin::registerTypes()"<<uri;

    qmlRegisterType<QmlTunerDummy>(uri, 1, 0, "QmlTunerDummy");
}

void QmlTunerPlugin::initializeEngine(QQmlEngine */*engine*/, const char *uri)
{
    qDebug()<<"[QMLTUNER] QmlTunerPlugin::initializeEngine()"<<uri;

    QmlTunerServer::init(qApp);
}
