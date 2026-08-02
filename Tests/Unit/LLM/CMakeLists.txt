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
set(RDK_LLM_ADVERSARIAL_FIXTURES_DIR "${CMAKE_SOURCE_DIR}/Rdk/Tests/Fixtures/LLM/adversarial")

_add_llm_unit_test(Test_LLM_ArgumentValidator test_llm_argument_validator.cpp)
_add_llm_unit_test(Test_LLM_AdversarialFixtures test_llm_adversarial_fixtures.cpp)
target_compile_definitions(Test_LLM_AdversarialFixtures PRIVATE
  RDK_LLM_ADVERSARIAL_FIXTURES_DIR="${RDK_LLM_ADVERSARIAL_FIXTURES_DIR}")
_add_llm_unit_test(Test_LLM_HttpRetry test_llm_http_retry.cpp)
_add_llm_unit_test(Test_LLM_IntentAmbiguityGate test_llm_intent_ambiguity_gate.cpp)
_add_llm_unit_test(Test_LLM_Policy test_llm_policy.cpp)
_add_llm_unit_test(Test_LLM_AutonomousPolicy test_llm_autonomous_policy.cpp)
_add_llm_unit_test(Test_LLM_QuantityParser test_llm_quantity_parser.cpp)
_add_llm_unit_test(Test_LLM_QuantityResolver test_llm_quantity_resolver.cpp)
_add_llm_unit_test(Test_LLM_TaskPlanParsing test_llm_task_plan_parsing.cpp)
_add_llm_unit_test(Test_LLM_TaskPlanner test_llm_task_planner.cpp)
_add_llm_unit_test(Test_LLM_TaskOutcomeSummary test_llm_task_outcome_summary.cpp)
_add_llm_unit_test(Test_LLM_LinkIdentity test_llm_link_identity.cpp)
_add_llm_unit_test(Test_LLM_ModelLinkWalker test_llm_model_link_walker.cpp)
_add_llm_unit_test(Test_LLM_ConnectSemanticsCatalog test_llm_connect_semantics_catalog.cpp)
_add_llm_unit_test(Test_LLM_ConnectPortInference test_llm_connect_port_inference.cpp)
_add_llm_unit_test(Test_LLM_ConnectEndpoints test_llm_connect_endpoints.cpp)
_add_llm_unit_test(Test_LLM_ConnectPairing test_llm_connect_pairing.cpp)
_add_llm_unit_test(Test_LLM_DomainSnapshotLinks test_llm_domain_snapshot_links.cpp)
_add_llm_unit_test(Test_LLM_ConnectPlanParsing test_llm_connect_plan_parsing.cpp)
_add_llm_unit_test(Test_LLM_ActOrClarifyGate test_llm_act_or_clarify_gate.cpp)
_add_llm_unit_test(Test_LLM_ChannelCalcCommand test_llm_channel_calc_command.cpp)
_add_llm_unit_test(Test_LLM_CompoundGoal test_llm_compound_goal.cpp)
_add_llm_unit_test(Test_LLM_ComponentStructureGoal test_llm_component_structure_goal.cpp)
_add_llm_unit_test(Test_LLM_WatchTools test_llm_watch_tools.cpp)
_add_llm_unit_test(Test_LLM_ChatNameCompletion test_llm_chat_name_completion.cpp)
_add_llm_unit_test(Test_LLM_DialogSlotMerge test_llm_dialog_slot_merge.cpp)
_add_llm_unit_test(Test_LLM_AgentManifest test_llm_agent_manifest.cpp)
_add_llm_unit_test(Test_LLM_ConnectPlanBuilder test_llm_connect_plan_builder.cpp)
_add_llm_unit_test(Test_LLM_SessionGraphMemory test_llm_session_graph_memory.cpp)
_add_llm_unit_test(Test_LLM_StepVerifier test_llm_step_verifier.cpp)
_add_llm_unit_test(Test_LLM_PostToolVerify test_llm_post_tool_verify.cpp)
_add_llm_unit_test(Test_LLM_WriteToolRepair test_llm_write_tool_repair.cpp)
_add_llm_unit_test(Test_LLM_QueryNormalizer test_llm_query_normalizer.cpp)
_add_llm_unit_test(Test_LLM_CtagsChunker test_llm_ctags_chunker.cpp)
_add_llm_unit_test(Test_LLM_YamlKnowledgeCatalog
  test_llm_yaml_knowledge_catalog.cpp
  ${CMAKE_SOURCE_DIR}/App/NeuroModeler/NmsdkBuiltinKnowledgeCatalog.cpp)
target_include_directories(Test_LLM_YamlKnowledgeCatalog PRIVATE ${CMAKE_SOURCE_DIR}/App/NeuroModeler)
_add_llm_unit_test(Test_LLM_TD041WalkthroughSim test_llm_td041_walkthrough_sim.cpp)
_add_llm_unit_test(Test_LLM_DynamicToolRouter test_llm_dynamic_tool_router.cpp)
_add_llm_unit_test(Test_LLM_LinkPatternCatalog test_llm_link_pattern_catalog.cpp)
_add_llm_unit_test(Test_LLM_WriteToolUserMessage test_llm_write_tool_user_message.cpp)
_add_llm_unit_test(Test_LLM_ActionIntentPreview test_llm_action_intent_preview.cpp)
_add_llm_unit_test(Test_LLM_IntentParser test_llm_intent_parser.cpp)
_add_llm_unit_test(Test_LLM_ConfigurationLifecycle test_llm_configuration_lifecycle.cpp)
_add_llm_unit_test(Test_LLM_LibraryScopeHint test_llm_library_scope_hint.cpp)
_add_llm_unit_test(Test_LLM_LifecycleArgumentGate test_llm_lifecycle_argument_gate.cpp)
_add_llm_unit_test(Test_LLM_WriteArgumentNormalizer test_llm_write_argument_normalizer.cpp)
_add_llm_unit_test(Test_LLM_NameResolution test_llm_name_resolution.cpp)
_add_llm_unit_test(Test_LLM_OrchestratorLifecycleArgs test_llm_orchestrator_lifecycle_args.cpp)
_add_llm_unit_test(Test_LLM_ToolGateway test_llm_tool_gateway.cpp)
_add_llm_unit_test(Test_LLM_ToolTrace test_llm_tool_trace.cpp)
_add_llm_unit_test(Test_LLM_GuiTurnPin test_llm_gui_turn_pin.cpp)
_add_llm_unit_test(Test_LLM_AddParentResolution test_llm_add_parent_resolution.cpp)
_add_llm_unit_test(Test_LLM_ClarificationFormat test_llm_clarification_format.cpp)
_add_llm_unit_test(Test_LLM_DirectAddFollowup test_llm_direct_add_followup.cpp)
_add_llm_unit_test(Test_LLM_RecordedToolInvoke test_llm_recorded_tool_invoke.cpp)
_add_llm_unit_test(Test_LLM_EntityPathCanonicalizer test_llm_entity_path_canonicalizer.cpp)
_add_llm_unit_test(Test_LLM_Orchestrator test_llm_orchestrator.cpp)
_add_llm_unit_test(Test_LLM_OrchestratorThinking test_llm_orchestrator_thinking.cpp
  llm_test_ollama_lab.cpp)
_add_llm_unit_test(Test_LLM_ProviderAuth test_llm_provider_auth.cpp)
_add_llm_unit_test(Test_LLM_EntityResolver test_llm_entity_resolver.cpp)
_add_llm_unit_test(Test_LLM_AuditChain test_llm_audit_chain.cpp)
_add_llm_unit_test(Test_LLM_SessionPersist test_llm_session_persist.cpp)
_add_llm_unit_test(Test_LLM_ContextCompactor test_llm_context_compactor.cpp)
_add_llm_unit_test(Test_LLM_ContextAssembler test_llm_context_assembler.cpp)
_add_llm_unit_test(Test_LLM_ContextKnowledgeBlocks test_llm_context_knowledge_blocks.cpp)
_add_llm_unit_test(Test_LLM_ContextAcquisitionPolicy test_llm_context_acquisition_policy.cpp)
_add_llm_unit_test(Test_LLM_CurrentComponentScope test_llm_current_component_scope.cpp)
_add_llm_unit_test(Test_LLM_LongTermMemory test_llm_long_term_memory.cpp)
_add_llm_unit_test(Test_LLM_ResolvedEntityStore test_llm_resolved_entity_store.cpp)
_add_llm_unit_test(Test_ReadOnlyLogTail test_read_only_log_tail.cpp)
_add_llm_unit_test(Test_LLM_SystemLogPolicy test_llm_system_log_policy.cpp)
target_include_directories(Test_ReadOnlyLogTail PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/Core
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Engine
  ${CMAKE_SOURCE_DIR}/Rdk/Core/Application)
_add_llm_unit_test(Test_LLM_ChatHistoryArchive
  test_llm_chat_history_archive.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm/ULlmChatHistoryArchive.cpp)
target_include_directories(Test_LLM_ChatHistoryArchive PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm)
_add_llm_unit_test(Test_LLM_ChatMarkdown
  test_llm_chat_markdown.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm/ULlmChatMarkdown.cpp)
target_include_directories(Test_LLM_ChatMarkdown PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm)
find_package(Qt5 COMPONENTS Gui REQUIRED)
target_link_libraries(Test_LLM_ChatMarkdown PRIVATE Qt5::Gui)
_add_llm_unit_test(Test_LLM_DetailsHtml
  test_llm_details_html.cpp
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm/ULlmDetailsHtml.cpp)
target_include_directories(Test_LLM_DetailsHtml PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/GUI/Qt/Llm)
target_link_libraries(Test_LLM_DetailsHtml PRIVATE Qt5::Core)
_add_llm_unit_test(Test_LLM_DocSearch test_llm_doc_search.cpp)
_add_llm_unit_test(Test_LLM_Workflow test_llm_workflow.cpp)
_add_llm_unit_test(Test_LLM_UnifiedTurnContract test_llm_unified_turn_contract.cpp)
_add_llm_unit_test(Test_LLM_FreeDialogueCorpus test_llm_free_dialogue_corpus.cpp)
target_compile_definitions(Test_LLM_FreeDialogueCorpus PRIVATE
  RDK_LLM_FREE_DIALOGUE_CORPUS_DIR="${CMAKE_SOURCE_DIR}/Rdk/Tests/Fixtures/LLM/free-dialogue-corpus")
_add_llm_unit_test(Test_LLM_AskUserProtocol test_llm_ask_user_protocol.cpp)
_add_llm_unit_test(Test_LLM_LifecycleSoftAndTerminal test_llm_lifecycle_soft_and_terminal.cpp)
_add_llm_unit_test(Test_LLM_SearchTools test_llm_search_tools.cpp)
_add_llm_unit_test(Test_LLM_Thinking test_llm_thinking.cpp)
_add_llm_unit_test(Test_LLM_ThinkingLab test_llm_thinking_lab.cpp llm_test_ollama_lab.cpp)
_add_llm_unit_test(Test_LLM_SubagentRunner test_llm_subagent_runner.cpp)
_add_llm_unit_test(Test_LLM_InputUnderstanding test_llm_input_understanding.cpp)
_add_llm_unit_test(Test_LLM_ModelRouter test_llm_model_router.cpp)
_add_llm_unit_test(Test_LLM_OllamaNative test_llm_ollama_native.cpp)
_add_llm_unit_test(Test_LLM_OllamaChatTemplate test_llm_ollama_chat_template.cpp)
_add_llm_unit_test(Test_LLM_ResponseLanguage test_llm_response_language.cpp)
_add_llm_unit_test(Test_LLM_UserMessages test_llm_user_messages.cpp)
_add_llm_unit_test(Test_LLM_OllamaModelInfo test_llm_ollama_model_info.cpp)
_add_llm_unit_test(Test_LLM_AuditSanitizer test_llm_audit_sanitizer.cpp)
_add_llm_unit_test(Test_LLM_ToolSchemaRegression test_llm_tool_schema_regression.cpp)
_add_llm_unit_test(Test_LLM_ExecutionPlan test_llm_execution_plan.cpp)
_add_llm_unit_test(Test_LLM_PlanExecutor test_llm_plan_executor.cpp)
_add_llm_unit_test(Test_LLM_PlanQuantity test_llm_plan_quantity.cpp)
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

set(RDK_LLM_AGENT_SCENARIOS_DIR "${CMAKE_SOURCE_DIR}/Rdk/Tests/Fixtures/LLM/agent-scenarios")
set(_LLM_AGENT_SCENARIO_SOURCES
  test_llm_agent_scenarios.cpp
  test_llm_agent_scenario_eval_unit.cpp
  llm_agent_scenario_loader.cpp
  llm_agent_scenario_runner.cpp
  llm_agent_scenario_eval.cpp
  llm_e2e_lab_harness.cpp
  llm_e2e_analyzer.cpp
  llm_test_ollama_lab.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm/RegisterPulseLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm/RegisterMotionControlLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Llm/RegisterHardwareLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm/RegisterBasicLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm/UPackBasicDocs.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm/RegisterCvBasicLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm/UPackCvBasicDocs.cpp)
_add_llm_unit_test(Test_LLM_AgentScenarios ${_LLM_AGENT_SCENARIO_SOURCES})
target_compile_definitions(Test_LLM_AgentScenarios PRIVATE
  RDK_LLM_AGENT_SCENARIOS_DIR="${RDK_LLM_AGENT_SCENARIOS_DIR}"
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(Test_LLM_AgentScenarios PRIVATE
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm)

set(_LLM_AGENT_SCENARIO_RUNNER_SOURCES
  llm_agent_scenario_loader.cpp
  llm_agent_scenario_runner.cpp
  llm_agent_scenario_eval.cpp
  llm_e2e_lab_harness.cpp
  llm_e2e_analyzer.cpp
  llm_test_ollama_lab.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm/RegisterPulseLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm/RegisterMotionControlLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Llm/RegisterHardwareLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm/RegisterBasicLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm/UPackBasicDocs.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm/RegisterCvBasicLibLlmTools.cpp
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm/UPackCvBasicDocs.cpp)
add_executable(llm_scenario_runner
  llm_scenario_runner_main.cpp
  ${_LLM_AGENT_SCENARIO_RUNNER_SOURCES}
  ${_LLM_TEST_COMMON})
target_link_libraries(llm_scenario_runner PRIVATE GTest::gtest rdk.llm.core rdk.static.qt)
target_include_directories(llm_scenario_runner PRIVATE
  ${CMAKE_SOURCE_DIR}/Rdk/LLM/Core
  ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
  ${CMAKE_SOURCE_DIR}/Rdk/Core
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-MotionControlLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-HardwareLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-BasicLib/Llm
  ${CMAKE_SOURCE_DIR}/Libraries/Rdk-CvBasicLib/Llm)
target_compile_definitions(llm_scenario_runner PRIVATE
  RDK_LLM_AGENT_SCENARIOS_DIR="${RDK_LLM_AGENT_SCENARIOS_DIR}"
  CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  RDK_LIB_TYPE= RDK_CALL=)
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
