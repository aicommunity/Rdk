cmake_minimum_required(VERSION 3.16)

project(rdk_static_qt LANGUAGES CXX)

set(TARGET_NAME rdk.static.qt)

add_library(${TARGET_NAME} STATIC)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/ThirdParty
)

# Sources taken from rdk.static.qt.pro (initially a reduced set)
file(GLOB RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Engine/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Graphics/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Math/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Serialize/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/System/Qt/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/System/Gcc/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/System/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Utilities/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/Qt/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/ThirdParty/xmlParser/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/ThirdParty/jpeg/*.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/ThirdParty/ThirdParty.cpp
)

# Add C-API implementations and RDK initialization (required by NeuroModeler)
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_cpp_init.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_engine_support.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_version.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_new.cpp
)

# Add deploy files from the .pro file
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk.qt.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/myrdk.qt.cpp
)

# Add Application files from the .pro file
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/UProjectDeployer.cpp
)

# Add GCC-specific files for Linux
if(UNIX)
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Core/System/Gcc/pevents.cpp
  )
endif()

# Compatibility exclusions (Linux):
# 1) keep rdk_init.cpp excluded to avoid duplicates with rdk_cpp_init.cpp
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Deploy/Include/rdk_init\\.cpp$")
# 2) Exclude files that depend on headers missing in the current configuration
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/ULocalProperty\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyInput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyOutput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UShare\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyIO\\.cpp$")
# 3) Exclude a specific Qt control until dependencies are checked
# UServerTransportHttp.cpp was returned to compilation because the class is used in UServerControl
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UEngineControlQt\\.cpp$")
# 4) Exclude jpge.cpp and jpgd.cpp, since they are included into JpegIntegration.cpp via #include
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/ThirdParty/jpeg/jpge\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/ThirdParty/jpeg/jpgd\\.cpp$")
# 5) Exclude UDllLoader.qt.cpp, because it is included into rdk_system.qt.cpp via #include
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Qt/UDllLoader\\.qt\\.cpp$")
# 6) Exclude GCC-specific files on Windows
if(WIN32)
  list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/USharedMemoryLoader\\.gcc\\.cpp$")
endif()
# Always include the Qt-dependent deployer (used by NeuroModeler)
# (previously was conditional on RDK_USE_SQL)
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UProjectDeployerQt\\.cpp$")
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/Qt/UProjectDeployerQt.cpp
)
# 4) Use the Qt variant of the system module, exclude the gcc variant
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/rdk_system\\.gcc\\.cpp$")
# Exclude the GCC version of the mutex and use the Qt version instead (QReadWriteLock::Recursive)
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/UGenericMutex\\.gcc\\.cpp$")

# Add only the necessary deploy files on Unix
if (UNIX)
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_exceptions.ansi.cpp
  )
elseif(WIN32)
  # Add GetSystemExceptionData implementation for Windows
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_exceptions.win.cpp
  )
endif()

target_sources(${TARGET_NAME}
  PRIVATE
    ${RDK_STATIC_SOURCES}
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_rpc.cpp
)

target_compile_definitions(${TARGET_NAME}
  PUBLIC
    RDK_QT
    LIBRDK_LIBRARY_EXPORT
    RDK_USE_SQL
)

# Configure Qt (Qt5/Qt6) via shared module
include(${CMAKE_SOURCE_DIR}/cmake/SelectQt.cmake)

# Qt dependencies for modules used in Core/Application
target_link_libraries(${TARGET_NAME} PUBLIC
  Nmsdk::Qt::Core
  Nmsdk::Qt::Network
  Nmsdk::Qt::Sql
)

# Boost libraries (required components)
# Use MODULE mode on all platforms (FindBoost.cmake correctly determines library names)
find_package(Boost QUIET COMPONENTS program_options thread filesystem system chrono atomic)
if(Boost_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC
    Boost::program_options
    Boost::thread
    Boost::filesystem
    Boost::system
    Boost::chrono
    Boost::atomic
  )
endif()

# rdk_rpc.cpp is added directly to target_sources above

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

# Winsock for Windows (used in UGlogMirrorSink.cpp)
if(WIN32)
  target_link_libraries(${TARGET_NAME} PUBLIC ws2_32)
endif()

# Google glog for logging
find_package(glog QUIET)
if(NOT glog_FOUND)
  # Try to find glog using pkg-config or standard paths
  find_path(GLOG_INCLUDE_DIR glog/logging.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/homebrew/include
  )
  find_library(GLOG_LIBRARY
    NAMES glog
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/homebrew/lib
  )
  if(GLOG_INCLUDE_DIR AND GLOG_LIBRARY)
    add_library(glog::glog UNKNOWN IMPORTED)
    set_target_properties(glog::glog PROPERTIES
      IMPORTED_LOCATION "${GLOG_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GLOG_INCLUDE_DIR}"
    )
    set(glog_FOUND TRUE)
  endif()
endif()
if(glog_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC glog::glog)
  target_compile_definitions(${TARGET_NAME} PUBLIC RDK_USE_GLOG)
else()
  message(WARNING "glog not found. Logging will use fallback implementation.")
endif()

if (RDK_USE_OPENCV AND OpenCV_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC ${OpenCV_LIBS})
endif()

# Output directories are already configured globally in the root CMakeLists

if(RDK_USE_LLM)
  add_subdirectory(LLM)
endif()

# Add tests subdirectory if BUILD_TESTING is enabled.
# Keep local testing enabled in this subproject so CTest traverses build/Debug/Rdk.
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(Tests)
endif()

