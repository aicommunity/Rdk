# Pending CMake / vcpkg changes (применить в Agent mode)

Документация ссылается на опции ниже. При старте фазы 0 кода применить патчи.

## 1. `cmake/RdkDefines.cmake`

После `option(RDK_USE_MATLAB ...)` добавить:

```cmake
option(RDK_USE_LLM "Build RDK LLM assistant module (Rdk/LLM, GUI/Llm)" OFF)
option(RDK_LLM_BUILD_EMBEDDED "Build embedded llama.cpp provider (requires RDK_USE_LLM)" OFF)
```

## 2. `vcpkg.json` — feature `llm`

В `"features"` добавить (curl уже в base `dependencies`):

```json
"llm": {
  "description": "RDK LLM module (Windows): nlohmann-json when RDK_USE_LLM=ON",
  "dependencies": [ "nlohmann-json" ]
}
```

## 3. `Rdk/CMakeLists.txt`

Перед `if(BUILD_TESTING)`:

```cmake
if(RDK_USE_LLM)
  add_subdirectory(LLM)
endif()
```

## 4. `Rdk/LLM/CMakeLists.txt`

См. полный файл в [Build.md](Build.md) §2.
