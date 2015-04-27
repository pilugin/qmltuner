TARGET = qmltuner

QMAKE_CXXFLAGS += -std=c++0x

HEADERS += \
    itempropertiesmodel.h \
    form.h \
    qmlitemstree.h

SOURCES += \
    form.cpp \
    itempropertiesmodel.cpp \
    main.cpp \
    qmlitemstree.cpp

FORMS += \
    form.ui

QT += widgets

QML_TUNER_DBUS = client
include( $$PWD/../dbus/dbus.pri )

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
