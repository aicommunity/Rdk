#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = librdk
TEMPLATE = lib

DEFINES += LIBRDK_LIBRARY

SOURCES += librdk.cpp \
    ../../../../Deploy/Include/rdk.qt.cpp \
    ../../../../Deploy/Include/rdk_initlib.cpp

HEADERS += librdk.h\
        librdk_global.h \
    ../../../../Deploy/Include/rdk.qt.h \
    ../../../../Deploy/Include/rdk_initlib.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE975DFAE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = librdk.dll
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
}
