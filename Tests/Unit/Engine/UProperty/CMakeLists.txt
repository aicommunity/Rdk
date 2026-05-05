# Basic tests for UProperty
add_executable(Test_UProperty_Basic Test_UProperty_Basic.cpp)

# Tests for property connections
add_executable(Test_UProperty_Connections Test_UProperty_Connections.cpp)

# Tests for container properties
add_executable(Test_UProperty_Containers Test_UProperty_Containers.cpp)

# Performance tests
add_executable(Test_UProperty_Performance Test_UProperty_Performance.cpp)

# Direct update visibility tests
add_executable(Test_UProperty_DirectUpdateVisibility Test_UProperty_DirectUpdateVisibility.cpp)

foreach(TARGET Test_UProperty_Basic Test_UProperty_Connections Test_UProperty_Containers Test_UProperty_Performance Test_UProperty_DirectUpdateVisibility)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Deploy/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core
    )

    target_link_libraries(${TARGET} PRIVATE
        GTest::gtest
        GTest::gtest_main
        rdk.static.qt
    )

    target_compile_definitions(${TARGET} PRIVATE
        RDK_QT
        RDK_LIB_TYPE=
        RDK_CALL=
    )

    # Register test in CTest
    add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
