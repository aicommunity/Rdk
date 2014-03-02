#-------------------------------------------------
#
# Project created by QtCreator 2014-01-29T15:50:56
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodeGenerator
TEMPLATE = app


SOURCES += main.cpp\
        CodeGenerator.cpp \
    settingsdialog.cpp

HEADERS  += CodeGenerator.h \
    settingsdialog.h

FORMS    += CodeGenerator.ui

unix {
    INSTALLS += target
    DESTDIR = $$PWD/../../../Bin/Platform/Linux/CodeGenerator
}

windows {
    DESTDIR = $$PWD/../../../Bin/Platform/Win/CodeGenerator
}
