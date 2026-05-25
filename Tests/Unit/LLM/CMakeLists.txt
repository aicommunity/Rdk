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
_add_llm_unit_test(Test_LLM_OllamaChatTemplate test_llm_ollama_chat_template.cpp)
_add_llm_unit_test(Test_LLM_OllamaModelInfo test_llm_ollama_model_info.cpp)
_add_llm_unit_test(Test_LLM_AuditSanitizer test_llm_audit_sanitizer.cpp)
_add_llm_unit_test(Test_LLM_ToolSchemaRegression test_llm_tool_schema_regression.cpp)
_add_llm_unit_test(Test_LLM_ExecutionPlan test_llm_execution_plan.cpp)
_add_llm_unit_test(Test_LLM_PlanExecutor test_llm_plan_executor.cpp)
_add_llm_unit_test(Test_LLM_PlanCompensation test_llm_plan_compensation.cpp)
_add_llm_unit_test(Test_LLM_PlanCheckpoint test_llm_plan_checkpoint.cpp)
_add_llm_unit_test(Test_LLM_RegressionIntent test_llm_regression_intent.cpp)
_add_llm_unit_test(Test_LLM_HttpSse test_llm_http_sse.cpp)
_add_llm_unit_test(Test_LLM_ConfirmationTtl test_llm_confirmation_ttl.cpp)
_add_llm_unit_test(Test_LLM_DocOllamaEmbeddings test_llm_doc_ollama_embeddings.cpp)
_add_llm_unit_test(Test_LLM_DocSearchIndex test_llm_doc_search_index.cpp)
_add_llm_unit_test(Test_LLM_EmbeddedProvider test_llm_embedded_provider.cpp)
if(RDK_LLM_BUILD_EMBEDDED)
  target_link_libraries(Test_LLM_EmbeddedProvider PRIVATE rdk.llm.embedded)
  target_compile_definitions(Test_LLM_EmbeddedProvider PRIVATE RDK_LLM_EMBEDDED=1)
endif()
