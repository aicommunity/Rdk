add_executable(Test_UContainer_Components
    Test_UContainer_Components.cpp
)

target_link_libraries(Test_UContainer_Components
    PRIVATE
    rdk.static.qt
    gtest
    gtest_main
)

target_include_directories(Test_UContainer_Components
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Deploy/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core
)

add_test(NAME UContainer_Components COMMAND Test_UContainer_Components)

