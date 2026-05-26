cmake_minimum_required(VERSION 3.16)

set(_LLM_TEST_COMMON
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include/RdkDummyLibraries.cpp
)

function(_add_llm_unit_test name)
  set(sources ${ARGN})
  add_executable(${name} ${sources} ${_LLM_TEST_COMMON})
  target_link_libraries(${name} PRIVATE GTest::gtest GTest::gtest_main rdk.llm.core rdk.static.qt)
  # Tests that use EmbeddedLlama provider (not Mock) must also link rdk.llm.embedded.
  if(TARGET rdk.llm.embedded AND "${name}" STREQUAL "Test_LLM_EmbeddedProvider")
    target_link_libraries(${name} PRIVATE rdk.llm.embedded)
  endif()
  target_include_directories(${name} PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Rdk/Core
  )
  target_compile_definitions(${name} PRIVATE RDK_LIB_TYPE= RDK_CALL=)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

set(RDK_LLM_REGRESSION_FIXTURES_DIR "${CMAKE_SOURCE_DIR}/Rdk/Tests/Fixtures/LLM/regression")

_add_llm_unit_test(Test_LLM_ArgumentValidator test_llm_argument_validator.cpp)
_add_llm_unit_test(Test_LLM_Policy test_llm_policy.cpp)
_add_llm_unit_test(Test_LLM_AutonomousPolicy test_llm_autonomous_policy.cpp)
_add_llm_unit_test(Test_LLM_WriteToolUserMessage test_llm_write_tool_user_message.cpp)
_add_llm_unit_test(Test_LLM_IntentParser test_llm_intent_parser.cpp)
_add_llm_unit_test(Test_LLM_ConfigurationLifecycle test_llm_configuration_lifecycle.cpp)
_add_llm_unit_test(Test_LLM_LibraryScopeHint test_llm_library_scope_hint.cpp)
_add_llm_unit_test(Test_LLM_LifecycleArgumentGate test_llm_lifecycle_argument_gate.cpp)
_add_llm_unit_test(Test_LLM_OrchestratorLifecycleArgs test_llm_orchestrator_lifecycle_args.cpp)
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
_add_llm_unit_test(Test_LLM_ResponseLanguage test_llm_response_language.cpp)
_add_llm_unit_test(Test_LLM_UserMessages test_llm_user_messages.cpp)
_add_llm_unit_test(Test_LLM_OllamaModelInfo test_llm_ollama_model_info.cpp)
_add_llm_unit_test(Test_LLM_AuditSanitizer test_llm_audit_sanitizer.cpp)
_add_llm_unit_test(Test_LLM_ToolSchemaRegression test_llm_tool_schema_regression.cpp)
_add_llm_unit_test(Test_LLM_ExecutionPlan test_llm_execution_plan.cpp)
_add_llm_unit_test(Test_LLM_PlanExecutor test_llm_plan_executor.cpp)
_add_llm_unit_test(Test_LLM_PlanCompensation test_llm_plan_compensation.cpp)
_add_llm_unit_test(Test_LLM_PlanCheckpoint test_llm_plan_checkpoint.cpp)
_add_llm_unit_test(Test_LLM_RegressionIntent test_llm_regression_intent.cpp)
_add_llm_unit_test(Test_LLM_RegressionFixtures test_llm_regression_fixtures.cpp)
target_compile_definitions(Test_LLM_RegressionFixtures PRIVATE
  RDK_LLM_REGRESSION_FIXTURES_DIR="${RDK_LLM_REGRESSION_FIXTURES_DIR}")
_add_llm_unit_test(Test_LLM_HttpSse test_llm_http_sse.cpp)
_add_llm_unit_test(Test_LLM_ConfirmationTtl test_llm_confirmation_ttl.cpp)
_add_llm_unit_test(Test_LLM_PathPolicy test_llm_path_policy.cpp)
_add_llm_unit_test(Test_LLM_ApplicationCommands test_llm_application_commands.cpp)
_add_llm_unit_test(Test_LLM_PresentationSink test_llm_presentation_sink_mock.cpp)
_add_llm_unit_test(Test_LLM_ApplicationFixtures test_llm_application_fixtures.cpp)
target_compile_definitions(Test_LLM_ApplicationFixtures PRIVATE
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
_add_llm_unit_test(Test_LLM_OllamaLabIntegration test_llm_ollama_lab_integration.cpp llm_test_ollama_lab.cpp)
_add_llm_unit_test(Test_LLM_E2eLabCommands test_llm_e2e_lab_commands.cpp llm_e2e_lab_harness.cpp llm_test_ollama_lab.cpp)
target_compile_definitions(Test_LLM_E2eLabCommands PRIVATE
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
_add_llm_unit_test(Test_LLM_E2eScenarios
  test_llm_e2e_scenarios.cpp
  llm_e2e_lab_harness.cpp
  llm_e2e_analyzer.cpp
  llm_e2e_scenarios.cpp
  llm_test_ollama_lab.cpp)
target_compile_definitions(Test_LLM_E2eScenarios PRIVATE
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
_add_llm_unit_test(Test_LLM_DocOllamaEmbeddings test_llm_doc_ollama_embeddings.cpp)
_add_llm_unit_test(Test_LLM_DocSearchIndex test_llm_doc_search_index.cpp)
_add_llm_unit_test(Test_LLM_KnowledgeIndex
  test_llm_knowledge_index.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp)
target_compile_definitions(Test_LLM_KnowledgeIndex PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(Test_LLM_KnowledgeIndex PRIVATE ${CMAKE_SOURCE_DIR}/App/NeuroModeler)
_add_llm_unit_test(Test_LLM_DocRetrieval
  test_llm_doc_retrieval.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp)
target_compile_definitions(Test_LLM_DocRetrieval PRIVATE
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  RDK_LLM_RETRIEVAL_FIXTURES="${CMAKE_SOURCE_DIR}/Rdk/Tests/Fixtures/llm_retrieval_expectations.json")
target_include_directories(Test_LLM_DocRetrieval PRIVATE ${CMAKE_SOURCE_DIR}/App/NeuroModeler)
_add_llm_unit_test(Test_LLM_IndexIncremental
  test_llm_index_incremental.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp)
target_compile_definitions(Test_LLM_IndexIncremental PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(Test_LLM_IndexIncremental PRIVATE ${CMAKE_SOURCE_DIR}/App/NeuroModeler)
_add_llm_unit_test(Test_LLM_KnowledgeCatalog
  test_llm_knowledge_catalog.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp)
target_compile_definitions(Test_LLM_KnowledgeCatalog PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(Test_LLM_KnowledgeCatalog PRIVATE ${CMAKE_SOURCE_DIR}/App/NeuroModeler)
_add_llm_unit_test(Test_LLM_WriteToolsAudit test_llm_write_tools_audit.cpp)
_add_llm_unit_test(Test_LLM_WriteToolsP1 test_llm_write_tools_p1.cpp)
target_compile_definitions(Test_LLM_WriteToolsP1 PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
_add_llm_unit_test(Test_LLM_WriteToolsP2
  test_llm_write_tools_p2.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm/RegisterPulseLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm/RegisterMotionControlLibLlmTools.cpp)
target_include_directories(Test_LLM_WriteToolsP2 PRIVATE
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm)
add_executable(Test_LLM_WriteToolsEngine
  test_llm_write_tools_engine.cpp
  ${CMAKE_SOURCE_DIR}/Tests/Integration/Support/LlmWriteToolsEngineInit.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Libraries.cpp)
target_link_libraries(Test_LLM_WriteToolsEngine PRIVATE GTest::gtest GTest::gtest_main
  rdk.llm.core rdk.static.qt
  Rdk-BasicLib.qt Rdk-CvBasicLib.qt Rdk-HardwareLib.qt
  Nmsdk-PulseLib.qt Nmsdk-MotionControlLib.qt)
target_include_directories(Test_LLM_WriteToolsEngine PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
  ${CMAKE_SOURCE_DIR}/Rdk/Core
  ${CMAKE_SOURCE_DIR}/Tests/Integration/Support
  ${CMAKE_SOURCE_DIR}/Libraries)
target_compile_definitions(Test_LLM_WriteToolsEngine PRIVATE
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" RDK_LIB_TYPE= RDK_CALL=)
add_test(NAME Test_LLM_WriteToolsEngine COMMAND Test_LLM_WriteToolsEngine)
if(WIN32)
  set(_LLM_ENGINE_TEST_CWD "${CMAKE_SOURCE_DIR}/Bin/Platform/Win")
else()
  set(_LLM_ENGINE_TEST_CWD "${CMAKE_SOURCE_DIR}/Bin/Platform/Linux")
endif()
set_tests_properties(Test_LLM_WriteToolsEngine PROPERTIES
  WORKING_DIRECTORY "${_LLM_ENGINE_TEST_CWD}")
set_target_properties(Test_LLM_WriteToolsEngine PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_LLM_ENGINE_TEST_CWD}")
_add_llm_unit_test(Test_LLM_EmbeddedProvider test_llm_embedded_provider.cpp)
if(RDK_LLM_BUILD_EMBEDDED)
  target_compile_definitions(Test_LLM_EmbeddedProvider PRIVATE RDK_LLM_EMBEDDED=1)
endif()
