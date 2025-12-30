add_executable(Test_UComponent_Properties
    Test_UComponent_Properties.cpp
)

add_executable(Test_UComponent_Lifecycle
    Test_UComponent_Lifecycle.cpp
)

foreach(TARGET Test_UComponent_Properties Test_UComponent_Lifecycle)
    target_link_libraries(${TARGET}
        PRIVATE
        rdk.static.qt
        GTest::gtest
        GTest::gtest_main
    )

    target_include_directories(${TARGET}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Deploy/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core
    )

    target_compile_definitions(${TARGET}
        PRIVATE
        RDK_QT
        RDK_LIB_TYPE=
        RDK_CALL=
    )

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
