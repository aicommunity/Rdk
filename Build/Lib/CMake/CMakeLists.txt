cmake_minimum_required (VERSION 2.8)
project(Rdk)

# Put all the archive targets into ${PROJECT_NAME}/Lib/

set(A_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../Bin/Platform/Linux/Libs/")
file(MAKE_DIRECTORY ${A_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${A_OUTPUT_DIR})

#Build Rdk options
option(BUILD_APPLICATION "Build Rdk with Application" OFF)

# Compiler options

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Source files

# Rdk/Core directory
set(C_ENGINE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Engine/")
set(C_ENGINE_FILES
	"${C_ENGINE_SOURCE_DIR}/UEngine.cpp"
	"${C_ENGINE_SOURCE_DIR}/UContainer.cpp"
	"${C_ENGINE_SOURCE_DIR}/UPropertyInput.cpp"
	"${C_ENGINE_SOURCE_DIR}/UADItem.cpp"
	"${C_ENGINE_SOURCE_DIR}/UItem.cpp"
	"${C_ENGINE_SOURCE_DIR}/UStorage.cpp"
	"${C_ENGINE_SOURCE_DIR}/UComponent.cpp"
	"${C_ENGINE_SOURCE_DIR}/ULocalProperty.cpp"
	"${C_ENGINE_SOURCE_DIR}/UEnvironment.cpp"
	"${C_ENGINE_SOURCE_DIR}/ULogger.cpp"
	"${C_ENGINE_SOURCE_DIR}/UController.cpp"
	"${C_ENGINE_SOURCE_DIR}/ULibrary.cpp"
	"${C_ENGINE_SOURCE_DIR}/UModule.cpp"
	"${C_ENGINE_SOURCE_DIR}/UXMLEnvSerialize.cpp"
	"${C_ENGINE_SOURCE_DIR}/UConnector.cpp"
	"${C_ENGINE_SOURCE_DIR}/UProperty.cpp"
	"${C_ENGINE_SOURCE_DIR}/UPropertyIO.cpp"
	"${C_ENGINE_SOURCE_DIR}/UPropertyOutput.cpp"
	"${C_ENGINE_SOURCE_DIR}/UTime.cpp"
	"${C_ENGINE_SOURCE_DIR}/UAModel.cpp"
	"${C_ENGINE_SOURCE_DIR}/UContainerDescription.cpp"
	"${C_ENGINE_SOURCE_DIR}/UNet.cpp"
	"${C_ENGINE_SOURCE_DIR}/UBinaryEnvSerialize.cpp"
	"${C_ENGINE_SOURCE_DIR}/UEPtr.cpp"
	"${C_ENGINE_SOURCE_DIR}/UELockPtr.cpp"
	"${C_ENGINE_SOURCE_DIR}/UEnvException.cpp"
	"${C_ENGINE_SOURCE_DIR}/UEnvSupport.cpp"
	"${C_ENGINE_SOURCE_DIR}/UELockVar.cpp"
	"${C_ENGINE_SOURCE_DIR}/Engine.cpp"
	"${C_ENGINE_SOURCE_DIR}/UPointer.cpp"
	"${C_ENGINE_SOURCE_DIR}/UComponentDescription.cpp"
	"${C_ENGINE_SOURCE_DIR}/UShare.cpp"
	"${C_ENGINE_SOURCE_DIR}/UItemData.cpp"
	"${C_ENGINE_SOURCE_DIR}/ULoggerEnv.cpp")
set(C_GRAPHICS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Graphics/")
set(C_GRAPHICS_FILES
	"${C_GRAPHICS_SOURCE_DIR}/UBitmapVector.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UGraphicsIO.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/Gui.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UGraphicsBinarySerialize.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/Kernel.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/Graphics.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UGraphics.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UDrawEngine.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UGraphicsXMLSerialize.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UFont.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UBitmap.cpp"
	"${C_GRAPHICS_SOURCE_DIR}/UAGraphics.cpp")
set(C_MATH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Math/")
set(C_MATH_FILES
	"${C_MATH_SOURCE_DIR}/MCorrelation.cpp"
	"${C_MATH_SOURCE_DIR}/MGraphics.cpp"
	"${C_MATH_SOURCE_DIR}/MVector.cpp"
	"${C_MATH_SOURCE_DIR}/MKalmanFilter.cpp"
	"${C_MATH_SOURCE_DIR}/MMatrixBase.cpp"
	"${C_MATH_SOURCE_DIR}/UMath_lite.cpp"
	"${C_MATH_SOURCE_DIR}/MUBinarySerialize.cpp"
	"${C_MATH_SOURCE_DIR}/MDKalmanFilter.cpp"
	"${C_MATH_SOURCE_DIR}/MUIOStream.cpp"
	"${C_MATH_SOURCE_DIR}/MGeometry.cpp"
	"${C_MATH_SOURCE_DIR}/MMatrix.cpp"
	"${C_MATH_SOURCE_DIR}/MCamera.cpp"
	"${C_MATH_SOURCE_DIR}/UMath.cpp"
	"${C_MATH_SOURCE_DIR}/MUXMLSerialize.cpp")
set(C_SERIALIZE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Serialize/")
set(C_SERIALIZE_FILES
	"${C_SERIALIZE_SOURCE_DIR}/UBinaryStdSerialize.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/UIOStream.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/UXMLStdSerialize.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/Serialize.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/Serialize_lite.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/USerStorageBinary.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/USerStorage.cpp"
	"${C_SERIALIZE_SOURCE_DIR}/USerStorageXML.cpp")
set(C_SYSTEM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/System/")
set(C_SYSTEM_FILES
	"${C_SYSTEM_SOURCE_DIR}/Gcc/pevents.cpp"
	"${C_SYSTEM_SOURCE_DIR}/Gcc/UGenericMutex.gcc.cpp"
	"${C_SYSTEM_SOURCE_DIR}/Gcc/USharedMemoryLoader.gcc.cpp"
	"${C_SYSTEM_SOURCE_DIR}/Gcc/rdk_system.gcc.cpp"
	"${C_SYSTEM_SOURCE_DIR}/UGenericMutex.cpp")
set(C_UTILITIES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Utilities/")
set(C_UTILITIES_FILES
	"${C_UTILITIES_SOURCE_DIR}/UIniFile.cpp"
	"${C_UTILITIES_SOURCE_DIR}/UTree.cpp"
	"${C_UTILITIES_SOURCE_DIR}/Utilities_lite.cpp"
	"${C_UTILITIES_SOURCE_DIR}/UTransferPacket.cpp"
	"${C_UTILITIES_SOURCE_DIR}/Utilities.cpp"
	"${C_UTILITIES_SOURCE_DIR}/UPtr.cpp"
	"${C_UTILITIES_SOURCE_DIR}/UTimeStamp.cpp"
	"${C_UTILITIES_SOURCE_DIR}/USupport.cpp"
	"${C_UTILITIES_SOURCE_DIR}/URegistry.cpp"
	"${C_UTILITIES_SOURCE_DIR}/UException.cpp")
# Rdk/Deploy directory
set(DEPLOY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Deploy/Include/")
set(DEPLOY_FILES
	"${DEPLOY_SOURCE_DIR}/myrdk.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk_cpp_initdll.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk_engine_support.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk_exceptions.gcc.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk_new.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk_rpc.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk.cpp"
	"${DEPLOY_SOURCE_DIR}/rdk.gcc.cpp")
# Rdk/ThirdParty directory
set(THIRDPARTY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/")
set(THIRDPARTY_FILES
	"${THIRDPARTY_SOURCE_DIR}/xmlParser/xmlParser.cpp"
	"${THIRDPARTY_SOURCE_DIR}/jpeg/JpegIntegration.cpp"
	"${THIRDPARTY_SOURCE_DIR}/ThirdParty.cpp")

# Rdk/Application
set(APPLICATION_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Application/")
set(APPLICATION_SOURCE_FILES
	"${APPLICATION_SOURCE_DIR}/UAppCore.cpp"
	"${APPLICATION_SOURCE_DIR}/UApplication.cpp"
	"${APPLICATION_SOURCE_DIR}/UBroadcasterInterface.cpp"
	"${APPLICATION_SOURCE_DIR}/UChannelProfiler.cpp"
	"${APPLICATION_SOURCE_DIR}/UEngineControl.cpp"
	"${APPLICATION_SOURCE_DIR}/UEngineControlThread.cpp"
	"${APPLICATION_SOURCE_DIR}/UEngineStateThread.cpp"
	"${APPLICATION_SOURCE_DIR}/UIVisualController.cpp"
	"${APPLICATION_SOURCE_DIR}/UProject.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcCommand.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcCommandInternal.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcDecoder.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcDecoderCommon.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcDecoderInternal.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcDispatcher.cpp"
	"${APPLICATION_SOURCE_DIR}/URpcDispatcherQueues.cpp"
	"${APPLICATION_SOURCE_DIR}/UServerControl.cpp"
	"${APPLICATION_SOURCE_DIR}/UServerTransport.cpp"
	"${APPLICATION_SOURCE_DIR}/UServerTransportTcp.cpp"
	)
	
# Put all the files together
set(SOURCE_FILES
	${C_ENGINE_FILES}
	${C_GRAPHICS_FILES}
	${C_MATH_FILES}
	${C_SERIALIZE_FILES}
	${C_SYSTEM_FILES}
	${C_UTILITIES_FILES}
	${DEPLOY_FILES}
	${THIRDPARTY_FILES}
	)
if(BUILD_APPLICATION)
	List(APPEND SOURCE_FILES ${APPLICATION_SOURCE_FILES})
endif()
# Headers
	
include_directories(
	${C_ENGINE_SOURCE_DIR}
	${C_GRAPHICS_SOURCE_DIR}
	${C_MATH_SOURCE_DIR}
	${C_SERIALIZE_SOURCE_DIR}
	${C_SYSTEM_SOURCE_DIR}
	${C_UTILITIES_SOURCE_DIR}
	${DEPLOY_SOURCE_DIR}
	${THIRDPARTY_SOURCE_DIR}
	)
if(BUILD_APPLICATION)
	List(APPEND include_directories ${APPLICATION_SOURCE_DIR})
endif()
# Create target file

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
