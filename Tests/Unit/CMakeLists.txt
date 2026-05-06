cmake_minimum_required(VERSION 3.16)

# Unit tests for Rdk Engine components
add_subdirectory(Engine)

add_executable(Test_ComponentGuiRegistry
    Test_ComponentGuiRegistry.cpp
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
)

target_compile_definitions(Test_ComponentGuiRegistry PRIVATE
    RDK_QT
    RDK_LIB_TYPE=
    RDK_CALL=
)

add_test(NAME Test_ComponentGuiRegistry COMMAND Test_ComponentGuiRegistry)
