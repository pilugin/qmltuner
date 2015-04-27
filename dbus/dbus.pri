QT += dbus

INCLUDEPATH += $$PWD

HEADERS += $$PWD/qmltunertypes.h
SOURCES += $$PWD/qmltunertypes.cpp

contains(QML_TUNER_DBUS, server) {
    HEADERS += $$PWD/qmltuneradaptor.h
    SOURCES += $$PWD/qmltuneradaptor.cpp
}
else:contains(QML_TUNER_DBUS, client) {
    HEADERS += $$PWD/qmltunerproxy.h
    SOURCES += $$PWD/qmltunerproxy.cpp
}
