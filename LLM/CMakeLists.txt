cmake_minimum_required(VERSION 3.16)

if(NOT RDK_USE_LLM)
  return()
endif()

set(TARGET_NAME rdk.llm.core)
add_library(${TARGET_NAME} STATIC)

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_USE_LLM=1)

target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
  ${CMAKE_SOURCE_DIR}/Rdk/Core
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
)

file(GLOB_RECURSE LLM_CORE_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core/*.cpp
)
target_sources(${TARGET_NAME} PRIVATE ${LLM_CORE_SOURCES})

find_package(CURL REQUIRED)
target_link_libraries(${TARGET_NAME} PUBLIC rdk.static.qt CURL::libcurl)

if(WIN32)
  find_package(nlohmann_json CONFIG REQUIRED)
else()
  find_package(nlohmann_json CONFIG QUIET)
  if(NOT nlohmann_json_FOUND)
    find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
      PATHS /usr/include /usr/local/include /usr/include/nlohmann)
    if(NOT NLOHMANN_JSON_INCLUDE_DIR)
      message(FATAL_ERROR "nlohmann/json.hpp not found. Install nlohmann-json3-dev (Linux).")
    endif()
    add_library(nlohmann_json_iface INTERFACE)
    target_include_directories(nlohmann_json_iface INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json_iface)
  endif()
endif()
target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)

if(RDK_LLM_BUILD_EMBEDDED)
  add_subdirectory(Embedded)
  target_link_libraries(${TARGET_NAME} PUBLIC rdk.llm.embedded)
  target_compile_definitions(${TARGET_NAME} PUBLIC RDK_LLM_EMBEDDED=1)
endif()
