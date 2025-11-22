add_executable(Test_UStorage_Objects
    Test_UStorage_Objects.cpp
)

target_link_libraries(Test_UStorage_Objects
    PRIVATE
    rdk.static.qt
    gtest
    gtest_main
)

target_include_directories(Test_UStorage_Objects
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Deploy/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core
)

add_test(NAME UStorage_Objects COMMAND Test_UStorage_Objects)
