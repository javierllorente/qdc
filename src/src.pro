#-------------------------------------------------
#
# Project created by QtCreator 2012-04-02T00:24:13
#
#-------------------------------------------------

QT += core gui webkit sql

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}

TARGET = qrae
TEMPLATE = app

VERSION = 0.4.99
DEFINES *= QRAE_VERSION=\\\"""$$VERSION"\\\""

!mac:unix {
    target.path = /usr/bin
    hicolor = /usr/share/icons/hicolor
    data_directory = /usr/share/qrae

    launcher.files = ../qrae.desktop
    launcher.path = /usr/share/applications

    hicolor128.files = ../icons/qrae.png
    hicolor128.path = $$hicolor/128x128/apps

    autostart.files = ../autostart/qrae.desktop
    autostart.path = $$data_directory/autostart

    INSTALLS += target \
        launcher \
        hicolor128 \
        autostart
}

win32 {
    RC_ICONS = ../icons/qrae.ico
    QMAKE_TARGET_COMPANY = Javier Llorente
    QMAKE_TARGET_PRODUCT = qRAE
    QMAKE_TARGET_DESCRIPTION = Diccionario castellano de la RAE
    QMAKE_TARGET_COPYRIGHT = Copyright (C) 2012-2015 Javier Llorente
}

osx {
    ICON = ../icons/qrae.icns
    LIBS += -framework CoreServices -framework CoreFoundation
}


SOURCES += main.cpp\
        mainwindow.cpp\
        libqrae.cpp \
    proxysettings.cpp \
    settings.cpp \
    history.cpp \
    searchwidget.cpp

HEADERS  += mainwindow.h\
         libqrae.h \
    proxysettings.h \
    settings.h \
    history.h \
    searchwidget.h

FORMS    += mainwindow.ui \
    settings.ui \
    searchwidget.ui

OTHER_FILES +=

RESOURCES = ../application.qrc
