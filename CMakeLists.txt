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
# 3) Исключаем HTTP транспорт и специфичный Qt контрол до проверки зависимостей
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/UServerTransportHttp\\.cpp$")
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UEngineControlQt\\.cpp$")
# Подключаем Qt-зависимый деплойер всегда (NeuroModeler его использует)
# (ранее был условным по RDK_USE_SQL)
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/Application/Qt/UProjectDeployerQt\\.cpp$")
list(APPEND RDK_STATIC_SOURCES
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application/Qt/UProjectDeployerQt.cpp
)
# 4) Используем Qt-вариант системного модуля, исключаем gcc-вариант
list(FILTER RDK_STATIC_SOURCES EXCLUDE REGEX "/Rdk/Core/System/Gcc/rdk_system\\.gcc\\.cpp$")

# Добавляем только необходимые deploy-файлы под Unix
if (UNIX)
  list(APPEND RDK_STATIC_SOURCES
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/rdk_exceptions.ansi.cpp
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

# Qt зависимости для модулей, используемых в Core/Application
find_package(Qt5 REQUIRED COMPONENTS Core Network)
target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Core Qt5::Network)

# QtSql требуется UProjectDeployerQt
find_package(Qt5 REQUIRED COMPONENTS Sql)
target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Sql)
find_package(Boost QUIET COMPONENTS program_options)
if(Boost_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC Boost::program_options)
endif()

# Boost.Thread для потоков в Core/Application
find_package(Boost QUIET COMPONENTS thread)
if(Boost_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC Boost::thread)
endif()

# rdk_rpc.cpp добавлен напрямую в target_sources выше

find_package(Threads REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC Threads::Threads)

if (RDK_USE_OPENCV AND OpenCV_FOUND)
  target_link_libraries(${TARGET_NAME} PUBLIC ${OpenCV_LIBS})
endif()

# Вывод артефактов уже задан глобально в корневом CMakeLists

# Add tests subdirectory if BUILD_TESTING is enabled
if(BUILD_TESTING)
  add_subdirectory(Tests)
endif()
 
