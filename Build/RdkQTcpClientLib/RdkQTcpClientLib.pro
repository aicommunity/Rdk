QT -= gui
QT += core

greaterThan(QT_MAJOR_VERSION, 4): QT += network

TARGET = RdkQTcpClientLib
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

linux {
    DESTDIR = $$PWD/../../../Bin/Platform/Linux/RdkQTcpClientLib
 INCLUDEPATH += $$(BOOST_PATH)/include
} else:windows {
    DESTDIR = $$PWD/../../../Bin/Platform/Win/RdkQTcpClientLib
message("Using "msvc-$$(VisualStudioVersion) compiler)
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../Rdk/Core/Engine/UELockPtr.cpp \
    ../../../Rdk/Core/Engine/UELockVar.cpp \
    ../../../Rdk/Core/Serialize/Serialize.cpp \
    ../../../Rdk/Core/Serialize/Serialize_lite.cpp \
    ../../../Rdk/Core/Serialize/UIOStream.cpp \
    ../../../Rdk/Core/Serialize/USerStorage.cpp \
    ../../../Rdk/Core/Serialize/USerStorageXML.cpp \
    ../../../Rdk/Core/Serialize/UXMLStdSerialize.cpp \
    ../../../Rdk/Core/System/UGenericMutex.cpp \
    ../../../Rdk/Core/System/Qt/rdk_system.qt.cpp \
    ../../../Rdk/Core/System/Qt/UGenericMutex.qt.cpp \
    #../../../Rdk/Core/System/Qt/UDllLoader.qt.cpp \
    ../../../Rdk/Core/Utilities/UException.cpp \
    ../../../Rdk/ThirdParty/xmlParser/xmlParser.cpp \
    ../../../Rdk/Core/Utilities/UTransferPacket.cpp \
    ../../../Rdk/Core/System/Gcc/pevents.cpp \
    RdkQTcpClientLib.cpp \
    RdkQTcpClient.cpp

HEADERS += \
    ../../../Rdk/Core/Engine/UELockPtr.h \
    ../../../Rdk/Core/Engine/UELockVar.h \
    ../../../Rdk/Core/Serialize/Serialize.h \
    ../../../Rdk/Core/Serialize/Serialize_lite.h \
    ../../../Rdk/Core/Serialize/UIOStream.h \
    ../../../Rdk/Core/Serialize/USerStorage.h \
    ../../../Rdk/Core/Serialize/USerStorageXML.h \
    ../../../Rdk/Core/Serialize/UXMLStdSerialize.h \
    ../../../Rdk/Core/System/UGenericMutex.h \
    ../../../Rdk/Core/System/UDllLoader.h \
    ../../../Rdk/Core/Utilities/UException.h \
    ../../../Rdk/ThirdParty/xmlParser/xmlParser.h \
    ../../../Rdk/Core/Utilities/UTransferPacket.h \
    ../../../Rdk/Core/System/Gcc/pevents.h \
    RdkQTcpClientLib.h \
    RdkQTcpClient.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
