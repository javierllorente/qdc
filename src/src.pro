#-------------------------------------------------
#
# Project created by QtCreator 2012-04-02T00:24:13
#
#-------------------------------------------------

QT += core gui webkit sql

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}

TARGET = qdc
mac: TARGET = qdc
TEMPLATE = app

VERSION = 1.0.0
DEFINES *= QDC_VERSION=\\\"""$$VERSION"\\\""

!mac:unix {
    target.path = /usr/bin
    hicolor = /usr/share/icons/hicolor
    data_directory = /usr/share/qdc

    launcher.files = ../qdc.desktop
    launcher.path = /usr/share/applications

    hicolor128.files = ../icons/qdc.png
    hicolor128.path = $$hicolor/128x128/apps

    autostart.files = ../autostart/qdc.desktop
    autostart.path = $$data_directory/autostart

    INSTALLS += target \
        launcher \
        hicolor128 \
        autostart
}

win32 {
    RC_ICONS = ../icons/qdc.ico
    QMAKE_TARGET_COMPANY = Javier Llorente
    QMAKE_TARGET_PRODUCT = qdc
    QMAKE_TARGET_DESCRIPTION = Diccionario castellano
    QMAKE_TARGET_COPYRIGHT = Copyright (C) 2013-2016 Javier Llorente
    QMAKE_POST_LINK += $$PWD/../installer/build.bat
}

osx {
    ICON = ../icons/qdc.icns
    LIBS += -framework CoreServices -framework CoreFoundation
}


SOURCES += main.cpp\
        mainwindow.cpp\
        libqrae.cpp \
    proxysettings.cpp \
    settings.cpp \
    history.cpp \
    searchwidget.cpp \
    systray.cpp

HEADERS  += mainwindow.h\
         libqrae.h \
    proxysettings.h \
    settings.h \
    history.h \
    searchwidget.h \
    systray.h

FORMS    += mainwindow.ui \
    settings.ui \
    searchwidget.ui

OTHER_FILES +=

RESOURCES = ../application.qrc
