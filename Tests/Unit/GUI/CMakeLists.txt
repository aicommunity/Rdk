cmake_minimum_required(VERSION 3.16)

add_executable(Test_UModernDiagramLinkRouter
    Test_UModernDiagramLinkRouter.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/UModernDiagramLinkRouter.cpp
)

target_include_directories(Test_UModernDiagramLinkRouter PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
)

target_link_libraries(Test_UModernDiagramLinkRouter PRIVATE
    GTest::gtest
    GTest::gtest_main
    Nmsdk::Qt::Core
    Nmsdk::Qt::Gui
)

add_test(NAME Test_UModernDiagramLinkRouter COMMAND Test_UModernDiagramLinkRouter)

add_executable(Test_UModernDiagramScopePath
    Test_UModernDiagramScopePath.cpp
)

target_include_directories(Test_UModernDiagramScopePath PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
)

target_link_libraries(Test_UModernDiagramScopePath PRIVATE
    GTest::gtest
    GTest::gtest_main
    Nmsdk::Qt::Core
)

add_test(NAME Test_UModernDiagramScopePath COMMAND Test_UModernDiagramScopePath)
