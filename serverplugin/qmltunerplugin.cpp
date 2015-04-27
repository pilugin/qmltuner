#include "qmltunerplugin.h"
#include "qmltunerserver.h"
#include <qdeclarative.h>
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

void QmlTunerPlugin::initializeEngine(QDeclarativeEngine */*engine*/, const char *uri)
{
    qDebug()<<"[QMLTUNER] QmlTunerPlugin::initializeEngine()"<<uri;

    QmlTunerServer::init(qApp);
}

Q_EXPORT_PLUGIN2(qmltuner, QmlTunerPlugin)
