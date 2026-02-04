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
)

# Добавляем Application файлы из .pro
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/UProjectDeployer.cpp
)

# Добавляем Gcc-специфичные файлы для Linux
if(UNIX)
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Core/System/Gcc/pevents.cpp
  )
endif()

# Исключения совместимости (Linux):
# 1) rdk_init.cpp держим исключённым, чтобы избежать дублей с rdk_cpp_init.cpp
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Deploy/Include/rdk_init\\.cpp$")
# 2) Исключаем файлы, зависящие от отсутствующих заголовков в текущей конфигурации
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/ULocalProperty\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyInput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyOutput\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UShare\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Engine/UPropertyIO\\.cpp$")
# 3) Исключаем специфичный Qt контрол до проверки зависимостей
# UServerTransportHttp.cpp возвращен в компиляцию, так как класс используется в UServerControl
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UEngineControlQt\\.cpp$")
# 4) Исключаем jpge.cpp и jpgd.cpp, так как они включены в JpegIntegration.cpp через #include
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/ThirdParty/jpeg/jpge\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/ThirdParty/jpeg/jpgd\\.cpp$")
# 5) Исключаем UDllLoader.qt.cpp, так как он включен в rdk_system.qt.cpp через #include
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Qt/UDllLoader\\.qt\\.cpp$")
# 6) Исключаем GCC-специфичные файлы на Windows
if(WIN32)
  list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/USharedMemoryLoader\\.gcc\\.cpp$")
endif()
# Подключаем Qt-зависимый деплойер всегда (NeuroModeler его использует)
# (ранее был условным по RDK_USE_SQL)
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UProjectDeployerQt\\.cpp$")
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/Qt/UProjectDeployerQt.cpp
)
# 4) Используем Qt-вариант системного модуля, исключаем gcc-вариант
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/rdk_system\\.gcc\\.cpp$")
# Исключаем GCC версию мьютекса, используем Qt версию (QReadWriteLock::Recursive)
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/UGenericMutex\\.gcc\\.cpp$")

# Добавляем только необходимые deploy-файлы под Unix
if (UNIX)
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_exceptions.ansi.cpp
  )
elseif(WIN32)
  # Добавляем реализацию GetSystemExceptionData для Windows
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

# Настройка Qt (Qt5/Qt6) через общий модуль
include(${CMAKE_SOURCE_DIR}/cmake/SelectQt.cmake)

# Qt зависимости для модулей, используемых в Core/Application
target_link_libraries(${TARGET_NAME} PUBLIC
  Nmsdk::Qt::Core
  Nmsdk::Qt::Network
  Nmsdk::Qt::Sql
)

# Boost libraries (required components)
# Используем MODULE режим для всех платформ (FindBoost.cmake правильно определяет имена библиотек)
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

# rdk_rpc.cpp добавлен напрямую в target_sources выше

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

# Winsock для Windows (используется в UGlogMirrorSink.cpp)
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

# Вывод артефактов уже задан глобально в корневом CMakeLists

# Add tests subdirectory if BUILD_TESTING is enabled
if(BUILD_TESTING)
  add_subdirectory(Tests)
endif()

