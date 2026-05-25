if(NOT RDK_LLM_BUILD_EMBEDDED)
  return()
endif()

include(FetchContent)

set(RDK_LLM_LLAMA_CPP_DIR "" CACHE PATH "Local llama.cpp source tree (skips FetchContent)")

if(RDK_LLM_LLAMA_CPP_DIR)
  set(_LLAMA_SOURCE_DIR "${RDK_LLM_LLAMA_CPP_DIR}")
else()
  set(_LLAMA_TAG "b4533" CACHE STRING "llama.cpp git tag for embedded provider")
  FetchContent_Declare(
    llama_cpp
    GIT_REPOSITORY https://github.com/ggml-org/llama.cpp.git
    GIT_TAG ${_LLAMA_TAG}
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSIVE TRUE
  )
  FetchContent_GetProperties(llama_cpp)
  if(NOT llama_cpp_POPULATED)
    FetchContent_Populate(llama_cpp)
  endif()
  set(_LLAMA_SOURCE_DIR "${llama_cpp_SOURCE_DIR}")
endif()

set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER OFF CACHE BOOL "" FORCE)
set(LLAMA_CURL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GGML_CUDA OFF CACHE BOOL "" FORCE)
set(GGML_METAL OFF CACHE BOOL "" FORCE)
set(GGML_VULKAN OFF CACHE BOOL "" FORCE)
set(GGML_OPENMP ON CACHE BOOL "" FORCE)

add_subdirectory(${_LLAMA_SOURCE_DIR} ${CMAKE_BINARY_DIR}/llama_cpp_build EXCLUDE_FROM_ALL)

set(TARGET_NAME rdk.llm.embedded)
add_library(${TARGET_NAME} STATIC
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded/ULlamaRuntime.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded/UEmbeddedLlamaProvider.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded/CreateEmbeddedLlamaProvider.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded/LlamaGgufPath.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded
)

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_LLM_EMBEDDED=1)

target_link_libraries(${TARGET_NAME} PUBLIC rdk.static.qt llama)

if(TARGET ggml)
  target_link_libraries(${TARGET_NAME} PUBLIC ggml)
endif()
