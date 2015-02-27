#-------------------------------------------------
#
# Project created by QtCreator 2012-04-02T00:24:13
#
#-------------------------------------------------

QT       += core gui webkit sql

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}

TARGET = qrae
TEMPLATE = app

win32: RC_ICONS = qrae.ico
osx: ICON = qrae.icns


SOURCES += main.cpp\
        mainwindow.cpp\
        libqrae.cpp

HEADERS  += mainwindow.h\
         libqrae.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES = application.qrc
