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
  target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded)
endif()

add_executable(llm_audit_verify ${CMAKE_SOURCE_DIR}/Rdk/LLM/Tools/llm_audit_verify_main.cpp)
target_link_libraries(llm_audit_verify PRIVATE ${TARGET_NAME})
target_include_directories(llm_audit_verify PRIVATE ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core)

add_executable(llm_index_pack
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Tools/IndexPack/main.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp)
target_link_libraries(llm_index_pack PRIVATE ${TARGET_NAME} rdk.static.qt Nmsdk::Boost)
target_compile_definitions(llm_index_pack PRIVATE RDK_LIB_TYPE= RDK_CALL=)
target_include_directories(llm_index_pack PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler
  ${CMAKE_SOURCE_DIR}/Rdk/Core)
add_custom_target(llm-index-pack
  COMMAND $<TARGET_FILE:llm_index_pack> ${CMAKE_SOURCE_DIR}
  DEPENDS llm_index_pack
  COMMENT "Build Bin/LLM/index prebuilt knowledge pack")
