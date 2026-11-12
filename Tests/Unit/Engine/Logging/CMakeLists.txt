# Tests for the logging system
add_executable(Test_Logging_Sinks Test_Logging_Sinks.cpp)

target_sources(Test_Logging_Sinks PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
)

target_include_directories(Test_Logging_Sinks PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Deploy/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core
)

target_link_libraries(Test_Logging_Sinks PRIVATE
    GTest::gtest
    GTest::gtest_main
    rdk.static.qt
)

target_compile_definitions(Test_Logging_Sinks PRIVATE
    RDK_QT
    RDK_LIB_TYPE=
    RDK_CALL=
)

add_test(NAME Test_Logging_Sinks COMMAND Test_Logging_Sinks)

