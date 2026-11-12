# Build and CI

## RU

## 1. CMake option

```cmake
# cmake/RdkDefines.cmake
option(RDK_USE_LLM "Build RDK LLM assistant module" OFF)  # ON on Nmsdk branch `llm`
option(RDK_LLM_BUILD_EMBEDDED "Build llama.cpp embedded provider" OFF)
```

```cmake
# Rdk/CMakeLists.txt
if(RDK_USE_LLM)
  add_subdirectory(LLM)
endif()
```

---

## EN

## 2. Target `rdk.llm.core`

`Rdk/LLM/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
if(NOT RDK_USE_LLM)
  return()
endif()

set(TARGET_NAME rdk.llm.core)
add_library(${TARGET_NAME} STATIC)

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_USE_LLM=1)

target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
)

file(GLOB_RECURSE LLM_CORE_SOURCES
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
      message(FATAL_ERROR "Install nlohmann-json3-dev (Debian/Ubuntu)")
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
```

---
