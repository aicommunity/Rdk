
QT       -= gui

TARGET = @NAMESPACE_NAME@-@LIBRARY_NAME@.qt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT

VERSION = $$system(hg parents --template '{rev}')
DEFINES += RDK_LIB_CVS_REVISION=$$VERSION

unix {
    DESTDIR = $$PWD/../../../../Bin/Platform/Linux/Lib.Qt
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win/Lib.Qt
}

HEADERS += \
    ../../Core/@HEADER_FILE_NAME@ \


SOURCES += \
    ../../Core/@CPP_FILE_NAME@ \

