# Direct update visibility (maintained — exercises UProperty::SetDataDirect contract)
add_executable(Test_UProperty_DirectUpdateVisibility Test_UProperty_DirectUpdateVisibility.cpp)

# Minimal stubs for UEngine/UEnvironment entry points (full apps use Libraries/Libraries.cpp)
target_sources(Test_UProperty_DirectUpdateVisibility PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
)

target_include_directories(Test_UProperty_DirectUpdateVisibility PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Deploy/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core
)

target_link_libraries(Test_UProperty_DirectUpdateVisibility PRIVATE
    GTest::gtest
    GTest::gtest_main
    rdk.static.qt
)

target_compile_definitions(Test_UProperty_DirectUpdateVisibility PRIVATE
    RDK_QT
    RDK_LIB_TYPE=
    RDK_CALL=
)

add_test(NAME Test_UProperty_DirectUpdateVisibility COMMAND Test_UProperty_DirectUpdateVisibility)

if(RDK_BUILD_LEGACY_ENGINE_UNIT_TESTS)
    add_executable(Test_UProperty_Basic Test_UProperty_Basic.cpp)
    add_executable(Test_UProperty_Connections Test_UProperty_Connections.cpp)
    add_executable(Test_UProperty_Containers Test_UProperty_Containers.cpp)
    add_executable(Test_UProperty_Performance Test_UProperty_Performance.cpp)

    foreach(TARGET Test_UProperty_Basic Test_UProperty_Connections Test_UProperty_Containers Test_UProperty_Performance)
        target_sources(${TARGET} PRIVATE
            ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
        )

        target_include_directories(${TARGET} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Deploy/Include
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core
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

        add_test(NAME ${TARGET} COMMAND ${TARGET})
    endforeach()
endif()
