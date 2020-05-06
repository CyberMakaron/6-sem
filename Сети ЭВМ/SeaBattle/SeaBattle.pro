QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SeaBattle
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        Controller.cpp \
        Field.cpp \
        FieldCoord.cpp \
        Images.cpp \
        Model.cpp \
        main.cpp \
        mainwindow.cpp \
        netdialog.cpp \
        network.cpp

HEADERS += \
        Controller.h \
        Field.h \
        FieldCoord.h \
        Images.h \
        Model.h \
        mainwindow.h \
        netdialog.h \
        network.h

FORMS += \
        mainwindow.ui \
        netdialog.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
