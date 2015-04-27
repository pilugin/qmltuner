#include <QtGui/QGuiApplication>
#include <QtCore/QCoreApplication>
#include <QtDebug>
#include "qmltunerserver.h"

int QGuiApplication::exec()
{
    qDebug()<<"[QMLTUNER] QApplication::exec() injection";
    QmlTunerServer::init(qApp);
    return QCoreApplication::exec();
}
