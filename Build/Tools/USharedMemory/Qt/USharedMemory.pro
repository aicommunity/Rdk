#-------------------------------------------------
#
# Project created by QtCreator 2013-07-25T17:18:38
#
#-------------------------------------------------

QT       -= gui

TARGET = USharedMemory
TEMPLATE = lib

DEFINES += USHAREDMEMORY_LIBRARY

SOURCES += usharedmemory.cpp

HEADERS += usharedmemory.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}                  


unix:!symbian {
    DESTDIR = $$PWD/../../../../Bin/Platform/Linux
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win
}
