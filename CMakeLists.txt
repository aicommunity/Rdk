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

# Источники взяты из rdk.static.qt.pro (сокращённый набор начально)
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

# Добавляем реализации C-API и инициализации RDK (нужны NeuroModeler)
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_cpp_init.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_engine_support.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_version.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_new.cpp
)

# Добавляем deploy-файлы из .pro
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk.qt.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/myrdk.qt.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_exceptions.gcc.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_rpc.cpp
)

# Добавляем Application файлы из .pro
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/UProjectDeployer.cpp
)

# Исключаем проблемные файлы
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/ANSI/.*\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Win/.*\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Mac/.*\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/rdk_system\\.gcc\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UEngineControlQt\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/ULocalProperty\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyOutput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyIO\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyInput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UShare\\.cpp$")
# list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UContainer\\.cpp$")
# list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UItem\\.cpp$")
# list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UNet\\.cpp$")
# list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UStorage\\.cpp$")
# list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Deploy/Include/rdk_engine_support\\.cpp$")

target_sources(${TARGET_NAME} PRIVATE ${RDK_STATIC_SOURCES})

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_QT RDK_LIB_CVS_REVISION=0)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Sql SerialPort)
find_package(Boost REQUIRED COMPONENTS program_options)

target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Core Qt5::Widgets Qt5::Network Qt5::Sql Qt5::SerialPort Boost::program_options Threads::Threads)

# Modern compiler flags for GNU
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -O3 -march=native -mtune=native
        -flto -fuse-linker-plugin
        -Wno-unused-function
        -Wno-deprecated-copy
        -frtti
    )
endif()

# Precompiled headers
target_precompile_headers(${TARGET_NAME} PRIVATE
    <memory>
    <string>
    <vector>
    <QApplication>
    <QWidget>
)
