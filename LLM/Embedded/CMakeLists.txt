if(NOT RDK_LLM_BUILD_EMBEDDED)
  return()
endif()

set(TARGET_NAME rdk.llm.embedded)
add_library(${TARGET_NAME} STATIC)
target_sources(${TARGET_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Embedded/ULlamaRuntimeStub.cpp
)
target_include_directories(${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
)
target_link_libraries(${TARGET_NAME} PUBLIC rdk.static.qt)
