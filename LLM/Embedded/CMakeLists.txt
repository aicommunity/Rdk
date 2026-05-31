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
set(LLAMA_ALL_WARNINGS OFF CACHE BOOL "" FORCE)
set(GGML_ALL_WARNINGS OFF CACHE BOOL "" FORCE)

function(nmsdk_sanitize_llama_targets)
  if(NOT MSVC)
    return()
  endif()
  get_cmake_property(_all_targets TARGETS)
  foreach(_tgt IN LISTS _all_targets)
    if(NOT TARGET ${_tgt})
      continue()
    endif()
    get_target_property(_dir ${_tgt} BINARY_DIR)
    if(_dir MATCHES "llama_cpp_build")
      set_target_properties(${_tgt} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
      target_compile_options(${_tgt} PRIVATE /w /wd4244)
      if(MSVC_VERSION LESS 1930)
        target_compile_options(${_tgt} PRIVATE /Zc:char8_t-)
      endif()
    endif()
  endforeach()
endfunction()

if(MSVC)
  set(_nmsdk_llama_saved_cxx "${CMAKE_CXX_FLAGS}")
  set(_nmsdk_llama_saved_c   "${CMAKE_C_FLAGS}")
  set(_nmsdk_llama_saved_std "${CMAKE_CXX_STANDARD}")
  get_directory_property(_nmsdk_llama_saved_dir_opts COMPILE_OPTIONS)
  set_directory_properties(PROPERTIES COMPILE_OPTIONS "/w;/wd4996;/wd4244")
  set(CMAKE_CXX_STANDARD 17)
  string(APPEND CMAKE_CXX_FLAGS " /w")
  string(APPEND CMAKE_C_FLAGS   " /w")
  if(MSVC_VERSION LESS 1930)
    string(APPEND CMAKE_CXX_FLAGS " /Zc:char8_t-")
    string(APPEND CMAKE_C_FLAGS   " /Zc:char8_t-")
  endif()
endif()

add_subdirectory(${_LLAMA_SOURCE_DIR} ${CMAKE_BINARY_DIR}/llama_cpp_build EXCLUDE_FROM_ALL)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${_nmsdk_llama_saved_cxx}")
  set(CMAKE_C_FLAGS   "${_nmsdk_llama_saved_c}")
  set(CMAKE_CXX_STANDARD "${_nmsdk_llama_saved_std}")
  if(_nmsdk_llama_saved_dir_opts)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "${_nmsdk_llama_saved_dir_opts}")
  else()
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "")
  endif()
endif()

nmsdk_sanitize_llama_targets()

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

if(MSVC AND MSVC_VERSION LESS 1930)
  target_compile_options(${TARGET_NAME} PRIVATE /Zc:char8_t-)
endif()

if(TARGET ggml)
  target_link_libraries(${TARGET_NAME} PUBLIC ggml)
endif()
