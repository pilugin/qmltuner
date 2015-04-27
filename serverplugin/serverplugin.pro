TARGET = qmltuner
TEMPLATE = lib

CONFIG += qt plugin
QT += qml quick widgets

QMAKE_CXXFLAGS += -std=c++0x

DEFINES += SIGNALSLOT_GRAPH

HEADERS += \
    qmlitemmanager.h \
    qmltunerplugin.h \
    qmltunerserver.h

SOURCES += \
    qmlitemmanager.cpp \
    qmltunerplugin.cpp \
    qmltunerserver.cpp \
    inject.cpp

OTHER_FILES += \
    qmldir \
    qmltuner.json

QML_TUNER_DBUS = server
include( $$PWD/../dbus/dbus.pri )

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGET

other.files = $$OTHER_FILES
other.path = $$target.path
INSTALLS += target other
