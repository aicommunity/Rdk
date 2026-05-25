cmake_minimum_required(VERSION 3.16)

set(_LLM_TEST_COMMON
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
)

function(_add_llm_unit_test name source)
  add_executable(${name} ${source} ${_LLM_TEST_COMMON})
  target_link_libraries(${name} PRIVATE GTest::gtest GTest::gtest_main rdk.llm.core rdk.static.qt)
  target_include_directories(${name} PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Rdk/Core
  )
  target_compile_definitions(${name} PRIVATE RDK_LIB_TYPE= RDK_CALL=)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

_add_llm_unit_test(Test_LLM_ArgumentValidator test_llm_argument_validator.cpp)
_add_llm_unit_test(Test_LLM_Policy test_llm_policy.cpp)
_add_llm_unit_test(Test_LLM_IntentParser test_llm_intent_parser.cpp)
_add_llm_unit_test(Test_LLM_ToolGateway test_llm_tool_gateway.cpp)
_add_llm_unit_test(Test_LLM_Orchestrator test_llm_orchestrator.cpp)
_add_llm_unit_test(Test_LLM_ProviderAuth test_llm_provider_auth.cpp)
_add_llm_unit_test(Test_LLM_EntityResolver test_llm_entity_resolver.cpp)
_add_llm_unit_test(Test_LLM_AuditChain test_llm_audit_chain.cpp)
_add_llm_unit_test(Test_LLM_SessionPersist test_llm_session_persist.cpp)
_add_llm_unit_test(Test_LLM_DocSearch test_llm_doc_search.cpp)
_add_llm_unit_test(Test_LLM_Workflow test_llm_workflow.cpp)
_add_llm_unit_test(Test_LLM_OllamaNative test_llm_ollama_native.cpp)
_add_llm_unit_test(Test_LLM_EmbeddedProvider test_llm_embedded_provider.cpp)
if(RDK_LLM_BUILD_EMBEDDED)
  target_link_libraries(Test_LLM_EmbeddedProvider PRIVATE rdk.llm.embedded)
  target_compile_definitions(Test_LLM_EmbeddedProvider PRIVATE RDK_LLM_EMBEDDED=1)
endif()
