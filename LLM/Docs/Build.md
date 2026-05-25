# Build and CI

## 1. CMake option

```cmake
# cmake/RdkDefines.cmake
option(RDK_USE_LLM "Build RDK LLM assistant module" OFF)
option(RDK_LLM_BUILD_EMBEDDED "Build llama.cpp embedded provider" OFF)
```

```cmake
# Rdk/CMakeLists.txt
if(RDK_USE_LLM)
  add_subdirectory(LLM)
endif()
```

---

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

## 3. Зависимости

### Windows (vcpkg only)

`vcpkg.json` feature:

```json
"llm": {
  "description": "LLM module: curl + nlohmann-json",
  "dependencies": ["curl", "nlohmann-json"]
}
```

Configure:

```bash
cmake -B build -DRDK_USE_LLM=ON -DVCPKG_MANIFEST_FEATURES=llm
```

### Linux (system packages, **no vcpkg**)

```bash
sudo apt install libcurl4-openssl-dev nlohmann-json3-dev
cmake -B build -DRDK_USE_LLM=ON
```

---

## 4. NeuroModeler

```cmake
if(RDK_USE_LLM)
  target_compile_definitions(NeuroModeler PRIVATE RDK_USE_LLM=1)
  file(GLOB LLM_GUI_SOURCES ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm/*.cpp)
  target_sources(NeuroModeler PRIVATE
    ${LLM_GUI_SOURCES}
    App/NeuroModeler/NmsdkLlmProjectContext.cpp
    App/NeuroModeler/NmsdkRegisterLlm.cpp
  )
  target_link_libraries(NeuroModeler PRIVATE rdk.llm.core)
endif()
```

---

## 5. OFF build guarantees

При `RDK_USE_LLM=OFF`:

- Нет `add_subdirectory(Rdk/LLM)`
- Нет `RDK_USE_LLM` define на NeuroModeler
- Нет LLM sources в target
- vcpkg feature `llm` не активируется

---

## 6. CI jobs

| Job | Matrix |
|-----|--------|
| `default` | Linux + Win, `RDK_USE_LLM=OFF`, full build + core tests |
| `llm-linux` | `RDK_USE_LLM=ON`, apt deps, L1–L7 tests |
| `llm-win` | `RDK_USE_LLM=ON`, vcpkg feature llm |
| `llm-embedded` | `RDK_LLM_BUILD_EMBEDDED=ON`, optional GPU skip |

---

## 7. Preprocessor

| Macro | Meaning |
|-------|---------|
| `RDK_USE_LLM` | LLM code compiled |
| `RDK_LLM_EMBEDDED` | Embedded provider linked |

Использование в app:

```cpp
#ifdef RDK_USE_LLM
  NmsdkRegisterLlm(application);
#endif
```
