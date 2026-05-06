cmake_minimum_required(VERSION 3.16)

include(${CMAKE_SOURCE_DIR}/cmake/SelectQt.cmake)

# Unit tests for Rdk Engine components
add_subdirectory(Engine)

add_executable(Test_ComponentGuiRegistry
    Test_ComponentGuiRegistry.cpp
    ../../GUI/Qt/UComponentFormRegistry.cpp
    ../../GUI/Qt/UComponentGuiService.cpp
    ../../GUI/Qt/UGenericComponentControllerWidget.cpp
    ../../GUI/Qt/UVisualControllerWidget.cpp
    ../../GUI/Qt/UGuiTelemetry.cpp
    ../../Deploy/Include/RdkDummyLibraries.cpp
)

target_include_directories(Test_ComponentGuiRegistry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../GUI/Qt
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Deploy/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core
)

target_link_libraries(Test_ComponentGuiRegistry PRIVATE
    GTest::gtest
    GTest::gtest_main
    rdk.static.qt
    Nmsdk::Qt::Core
    Nmsdk::Qt::Widgets
)

target_compile_definitions(Test_ComponentGuiRegistry PRIVATE
    RDK_QT
    RDK_LIB_TYPE=
    RDK_CALL=
)

add_test(NAME Test_ComponentGuiRegistry COMMAND Test_ComponentGuiRegistry)
