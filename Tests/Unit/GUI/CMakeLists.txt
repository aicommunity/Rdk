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

add_executable(Test_WatchTemplateStore
    Test_WatchTemplateStore.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Plot/PlotDocument.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Plot/WatchTemplateStore.cpp
)

target_include_directories(Test_WatchTemplateStore PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
    ${CMAKE_SOURCE_DIR}/Rdk
)

target_link_libraries(Test_WatchTemplateStore PRIVATE
    GTest::gtest
    GTest::gtest_main
    Nmsdk::Qt::Core
    Nmsdk::Qt::Gui
    rdk.static.qt
)

add_test(NAME Test_WatchTemplateStore COMMAND Test_WatchTemplateStore)

add_executable(Test_PlotDecimation
    Test_PlotDecimation.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Plot/PlotDataAdapter.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Plot/PlotDocument.cpp
)

target_include_directories(Test_PlotDecimation PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
    ${CMAKE_SOURCE_DIR}/Rdk
)

target_link_libraries(Test_PlotDecimation PRIVATE
    GTest::gtest
    GTest::gtest_main
    Nmsdk::Qt::Core
    Nmsdk::Qt::Gui
    rdk.static.qt
)

add_test(NAME Test_PlotDecimation COMMAND Test_PlotDecimation)

add_executable(Test_WatchSeriesIdentity
    Test_WatchSeriesIdentity.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/UWatchSerie.cpp
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Plot/PlotDocument.cpp
)

target_include_directories(Test_WatchSeriesIdentity PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt
    ${CMAKE_SOURCE_DIR}/Rdk
)

target_link_libraries(Test_WatchSeriesIdentity PRIVATE
    GTest::gtest
    GTest::gtest_main
    Nmsdk::Qt::Core
    Nmsdk::Qt::Gui
    Nmsdk::Qt::Charts
    rdk.static.qt
)

add_test(NAME Test_WatchSeriesIdentity COMMAND Test_WatchSeriesIdentity)
