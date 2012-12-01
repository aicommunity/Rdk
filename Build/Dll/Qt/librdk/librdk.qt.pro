#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = librdk.qt
TEMPLATE = lib

DEFINES += LIBRDK_LIBRARY

SOURCES += librdk.qt.cpp \
    ../../../../Deploy/Include/rdk.qt.cpp \
    ../../../../Deploy/Include/myrdk.qt.cpp \
    ../../../../Deploy/Include/rdk_new.cpp \
    ../../../../Deploy/Include/rdk_cpp_initdll.cpp

HEADERS += librdk.qt.h\
        librdk.qt_global.h \
    ../../../../Deploy/Include/rdk.qt.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE975DFAE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = librdk.qt.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
    DESTDIR = $$PWD/../../../../Bin/Platform/Unix
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win
}
