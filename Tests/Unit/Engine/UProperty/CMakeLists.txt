# Базовые тесты для UProperty
add_executable(Test_UProperty_Basic Test_UProperty_Basic.cpp)

# Тесты для подключений свойств
add_executable(Test_UProperty_Connections Test_UProperty_Connections.cpp)

# Тесты для контейнерных свойств
add_executable(Test_UProperty_Containers Test_UProperty_Containers.cpp)

# Тесты производительности
add_executable(Test_UProperty_Performance Test_UProperty_Performance.cpp)

foreach(TARGET Test_UProperty_Basic Test_UProperty_Connections Test_UProperty_Containers Test_UProperty_Performance)
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

    # Добавляем тест в CTest
    add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
