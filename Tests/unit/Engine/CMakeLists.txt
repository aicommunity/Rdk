# Rdk Engine Unit Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for Engine tests
add_executable(RdkEngineTests
    TestUStorage.cpp
    TestUEnvironment.cpp
    TestUContainer.cpp
    TestUEngine.cpp
    TestUComponent.cpp
    TestUStorageLifecycle.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(RdkEngineTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Core/Engine
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Tests
    ${CMAKE_SOURCE_DIR}/Libraries
)

target_link_libraries(RdkEngineTests PRIVATE
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

target_compile_features(RdkEngineTests PRIVATE cxx_std_20)
target_compile_definitions(RdkEngineTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME RdkEngineTests COMMAND RdkEngineTests)

