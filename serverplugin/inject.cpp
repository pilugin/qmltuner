#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>
#include <QtDebug>
#include "qmltunerserver.h"

int QApplication::exec()
{
    qDebug()<<"[QMLTUNER] QApplication::exec() injection";
    QmlTunerServer::init(qApp);
    return QCoreApplication::exec();
}
