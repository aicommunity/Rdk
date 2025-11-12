# Rdk Engine Integration Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for Engine integration tests
add_executable(RdkEngineIntegrationTests
    TestEngineIntegration.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(RdkEngineIntegrationTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Core/Engine
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Tests
)

target_link_libraries(RdkEngineIntegrationTests PRIVATE
    rdk.static.qt
    Rdk-BasicLib.qt
    Rdk-CvBasicLib.qt
    Rdk-HardwareLib.qt
    Nmsdk-PulseLib.qt
    Nmsdk-MotionControlLib.qt
    TestHelpers
    GTest::gtest
    GTest::gtest_main
    glog::glog
    Threads::Threads
)

target_compile_features(RdkEngineIntegrationTests PRIVATE cxx_std_20)
target_compile_definitions(RdkEngineIntegrationTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME RdkEngineIntegrationTests COMMAND RdkEngineIntegrationTests)

