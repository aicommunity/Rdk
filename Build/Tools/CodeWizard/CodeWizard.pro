#-------------------------------------------------
#
# Project created by QtCreator 2013-07-06T15:59:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeWizard
TEMPLATE = app


SOURCES += main.cpp\
        codewizard.cpp

HEADERS  += codewizard.h

unix {
        target.path = /usr/lib
    INSTALLS += target
    DESTDIR = $$PWD/../../../Bin/Platform/Unix
}
windows {
    DESTDIR = $$PWD/../../../Bin/Platform/Win
}
