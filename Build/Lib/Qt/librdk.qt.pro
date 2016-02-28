#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = rdk.qt
TEMPLATE = lib
CONFIG += staticlib

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT

SOURCES += librdk.qt.cpp \
    ../../../Deploy/Include/rdk.qt.cpp \
    ../../../Deploy/Include/myrdk.qt.cpp \
    ../../../Core/Engine/Engine.cpp \
    ../../../Core/Engine/UADItem.cpp \
    ../../../Core/Engine/UAModel.cpp \
    ../../../Core/Engine/UBinaryEnvSerialize.cpp \
    ../../../Core/Engine/UComponent.cpp \
    ../../../Core/Engine/UComponentDescription.cpp \
    ../../../Core/Engine/UConnector.cpp \
    ../../../Core/Engine/UContainer.cpp \
    ../../../Core/Engine/UContainerDescription.cpp \
    ../../../Core/Engine/UController.cpp \
    ../../../Core/Engine/UELockPtr.cpp \
    ../../../Core/Engine/UELockVar.cpp \
    ../../../Core/Engine/UEngine.cpp \
    ../../../Core/Engine/UEnvException.cpp \
    ../../../Core/Engine/UEnvironment.cpp \
    ../../../Core/Engine/UEnvSupport.cpp \
    ../../../Core/Engine/UEPtr.cpp \
    ../../../Core/Engine/UItem.cpp \
    ../../../Core/Engine/UItemData.cpp \
    ../../../Core/Engine/ULibrary.cpp \
    ../../../Core/Engine/ULocalProperty.cpp \
    ../../../Core/Engine/ULogger.cpp \
    ../../../Core/Engine/UModule.cpp \
    ../../../Core/Engine/UNet.cpp \
    ../../../Core/Engine/UPointer.cpp \
    ../../../Core/Engine/UProperty.cpp \
    ../../../Core/Engine/UPropertyInput.cpp \
    ../../../Core/Engine/UPropertyIO.cpp \
    ../../../Core/Engine/UPropertyOutput.cpp \
    ../../../Core/Engine/UShare.cpp \
    ../../../Core/Engine/UStorage.cpp \
    ../../../Core/Engine/UTime.cpp \
    ../../../Core/Engine/UXMLEnvSerialize.cpp \
    ../../../Core/Graphics/Graphics.cpp \
    ../../../Core/Graphics/Gui.cpp \
    ../../../Core/Graphics/Kernel.cpp \
    ../../../Core/Graphics/UAGraphics.cpp \
    ../../../Core/Graphics/UBitmap.cpp \
    ../../../Core/Graphics/UBitmapVector.cpp \
    ../../../Core/Graphics/UDrawEngine.cpp \
    ../../../Core/Graphics/UFont.cpp \
    ../../../Core/Graphics/UGraphics.cpp \
    ../../../Core/Graphics/UGraphicsBinarySerialize.cpp \
    ../../../Core/Graphics/UGraphicsIO.cpp \
    ../../../Core/Graphics/UGraphicsXMLSerialize.cpp \
    ../../../Core/Math/MCamera.cpp \
    ../../../Core/Math/MCorrelation.cpp \
    ../../../Core/Math/MDKalmanFilter.cpp \
    ../../../Core/Math/MGeometry.cpp \
    ../../../Core/Math/MGraphics.cpp \
    ../../../Core/Math/MKalmanFilter.cpp \
    ../../../Core/Math/MMatrix.cpp \
    ../../../Core/Math/MMatrixBase.cpp \
    ../../../Core/Math/MUBinarySerialize.cpp \
    ../../../Core/Math/MUIOStream.cpp \
    ../../../Core/Math/MUXMLSerialize.cpp \
    ../../../Core/Math/MVector.cpp \
    ../../../Core/Math/UMath.cpp \
    ../../../Core/Math/UMath_lite.cpp \
    ../../../Core/Serialize/Serialize.cpp \
    ../../../Core/Serialize/Serialize_lite.cpp \
    ../../../Core/Serialize/UBinaryStdSerialize.cpp \
    ../../../Core/Serialize/UIOStream.cpp \
    ../../../Core/Serialize/USerStorage.cpp \
    ../../../Core/Serialize/USerStorageBinary.cpp \
    ../../../Core/Serialize/USerStorageXML.cpp \
    ../../../Core/Serialize/UXMLStdSerialize.cpp \
    ../../../Core/System/UGenericMutex.cpp \
    ../../../Core/System/Qt/rdk_system.qt.cpp \
    ../../../Core/System/Qt/UGenericMutex.qt.cpp \
    ../../../Core/System/Qt/USharedMemoryLoader.qt.cpp \
    ../../../Core/Utilities/UException.cpp \
    ../../../Core/Utilities/UIniFile.cpp \
    ../../../Core/Utilities/UPtr.cpp \
    ../../../Core/Utilities/URegistry.cpp \
    ../../../Core/Utilities/USupport.cpp \
    ../../../Core/Utilities/Utilities.cpp \
    ../../../Core/Utilities/Utilities_lite.cpp \
    ../../../Core/Utilities/UTimeStamp.cpp \
    ../../../Core/Utilities/UTransferPacket.cpp \
    ../../../Core/Utilities/UTree.cpp \
    ../../../ThirdParty/ThirdParty.cpp \
    ../../../ThirdParty/xmlParser/xmlParser.cpp \
    ../../../ThirdParty/jpeg/JpegIntegration.cpp \
    ../../../ThirdParty/jpeg/jpgd.cpp \
    ../../../ThirdParty/jpeg/jpge.cpp \
    ../../../Deploy/Include/rdk_rpc.cpp \
    ../../../Deploy/Include/myrdk.cpp \
    ../../../Deploy/Include/rdk.cpp \
    ../../../Deploy/Include/rdk_cpp_initdll.cpp \
    ../../../Deploy/Include/rdk_engine_support.cpp \
    ../../../Deploy/Include/rdk_exceptions.ansi.cpp \
    ../../../Deploy/Include/rdk_initdll.cpp \
    ../../../Deploy/Include/rdk_cpp_init.cpp \
    ../../../Deploy/Include/rdk_init.cpp \
    ../../../Deploy/Include/rdk_new.cpp \
    ../../../Core/Application/UIVisualController.cpp

HEADERS += librdk.qt.h\
        librdk.qt_global.h \
    ../../../Deploy/Include/rdk.qt.h \
    ../../../Core/Engine/Engine.h \
    ../../../Core/Engine/Engine.h.orig \
    ../../../Core/Engine/UADItem.h \
    ../../../Core/Engine/UAModel.h \
    ../../../Core/Engine/UBinaryEnvSerialize.h \
    ../../../Core/Engine/UComponent.h \
    ../../../Core/Engine/UComponentDescription.h \
    ../../../Core/Engine/UConnector.h \
    ../../../Core/Engine/UContainer.h \
    ../../../Core/Engine/UContainerDescription.h \
    ../../../Core/Engine/UController.h \
    ../../../Core/Engine/UELockPtr.h \
    ../../../Core/Engine/UELockVar.h \
    ../../../Core/Engine/UEngine.h \
    ../../../Core/Engine/UEnvException.h \
    ../../../Core/Engine/UEnvironment.h \
    ../../../Core/Engine/UEnvSupport.h \
    ../../../Core/Engine/UEPtr.h \
    ../../../Core/Engine/UItem.h \
    ../../../Core/Engine/UItemData.h \
    ../../../Core/Engine/ULibrary.h \
    ../../../Core/Engine/ULocalProperty.h \
    ../../../Core/Engine/ULogger.h \
    ../../../Core/Engine/UModule.h \
    ../../../Core/Engine/UNet.h \
    ../../../Core/Engine/UPointer.h \
    ../../../Core/Engine/UProperty.h \
    ../../../Core/Engine/UPropertyInput.h \
    ../../../Core/Engine/UPropertyIO.h \
    ../../../Core/Engine/UPropertyOutput.h \
    ../../../Core/Engine/UShare.h \
    ../../../Core/Engine/UStorage.h \
    ../../../Core/Engine/UTime.h \
    ../../../Core/Engine/UXMLEnvSerialize.h \
    ../../../Core/Graphics/Graphics.h \
    ../../../Core/Graphics/Gui.h \
    ../../../Core/Graphics/Kernel.h \
    ../../../Core/Graphics/UAGraphics.h \
    ../../../Core/Graphics/UBitmap.h \
    ../../../Core/Graphics/UBitmapVector.h \
    ../../../Core/Graphics/UDrawEngine.h \
    ../../../Core/Graphics/UFont.h \
    ../../../Core/Graphics/UGraphics.h \
    ../../../Core/Graphics/UGraphicsBinarySerialize.h \
    ../../../Core/Graphics/UGraphicsIO.h \
    ../../../Core/Graphics/UGraphicsXMLSerialize.h \
    ../../../Core/Math/MCamera.h \
    ../../../Core/Math/MCorrelation.h \
    ../../../Core/Math/MDKalmanFilter.h \
    ../../../Core/Math/MDMatrix.h \
    ../../../Core/Math/MDVector.h \
    ../../../Core/Math/MGeometry.h \
    ../../../Core/Math/MGraphics.h \
    ../../../Core/Math/MKalmanFilter.h \
    ../../../Core/Math/MMatrix.h \
    ../../../Core/Math/MMatrixBase.h \
    ../../../Core/Math/MUBinarySerialize.h \
    ../../../Core/Math/MUIOStream.h \
    ../../../Core/Math/MUXMLSerialize.h \
    ../../../Core/Math/MVector.h \
    ../../../Core/Math/UMath.h \
    ../../../Core/Math/UMath_lite.h \
    ../../../Core/Serialize/Serialize.h \
    ../../../Core/Serialize/Serialize_lite.h \
    ../../../Core/Serialize/UBinaryStdSerialize.h \
    ../../../Core/Serialize/UIOStream.h \
    ../../../Core/Serialize/USerStorage.h \
    ../../../Core/Serialize/USerStorageBinary.h \
    ../../../Core/Serialize/USerStorageXML.h \
    ../../../Core/Serialize/UXMLStdSerialize.h \
    ../../../Core/System/rdk_system.h \
    ../../../Core/System/UGenericMutex.h \
    ../../../Core/System/USharedMemoryLoader.h \
    ../../../Core/Utilities/UException.h \
    ../../../Core/Utilities/UIniFile.h \
    ../../../Core/Utilities/UPtr.h \
    ../../../Core/Utilities/UQueue.h \
    ../../../Core/Utilities/URegistry.h \
    ../../../Core/Utilities/USupport.h \
    ../../../Core/Utilities/Utilities.h \
    ../../../Core/Utilities/Utilities_lite.h \
    ../../../Core/Utilities/UTimeStamp.h \
    ../../../Core/Utilities/UTransferPacket.h \
    ../../../Core/Utilities/UTree.h \
    ../../../ThirdParty/ThirdParty.h \
    ../../../ThirdParty/xmlParser/xmlParser.h \
    ../../../ThirdParty/jpeg/JpegIntegration.h \
    ../../../ThirdParty/jpeg/jpgd.h \
    ../../../ThirdParty/jpeg/jpge.h \
    ../../../Core/Application/UIVisualController.h \
    ../../../Deploy/Include/initdll_defs.h \
    ../../../Deploy/Include/rdk_engine_support.h \
    ../../../Deploy/Include/rdk_error_codes.h \
    ../../../Deploy/Include/rdk_exceptions.h \
    ../../../Deploy/Include/myrdk.h

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
    DESTDIR = $$PWD/../../../../Bin/Platform/Unix/Lib.Qt
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win/Lib.Qt
}
