# RDK LLM — Technical Debt Log

## RU

Living document. Update **after every phase** (see [Docs/Development-Workflow.md](Docs/Development-Workflow.md)).

**Rules:**
- Any deferred decision, shortcut, or known gap → add a row **before** phase commit.
- At phase end: try to close items marked `can_resolve_now`; re-prioritize the rest.
- Do not delete rows; set `Status` to `done` / `cancelled` with date.
- Any “we will decide later” must be captured either:
  - as a TD row (if it implies work), or
  - as an entry in **Deferred decisions log** (if it is an ADR-style choice to revisit).

---

## EN

## Status legend

| Status | Meaning |
|--------|---------|
| `open` | Not started |
| `in_progress` | Actively worked this phase |
| `done` | Resolved (note commit or PR in Resolution) |
| `cancelled` | No longer needed |
| `blocked` | External dependency |
| `deferred` | Explicitly shelved; no active sprint (see Notes / Deferred decisions) |

| Priority | Meaning |
|----------|---------|
| P0 | Blocks next phase |
| P1 | Should fix next phase |
| P2 | Nice to have |
| P3 | Post-MVP |

---

## Open items

| ID | Item | Target | Priority | Status | Notes |
|----|------|--------|----------|--------|-------|
| TD-072 | Sync Agentic-Risk-Test-Matrix after audit (TD-071 covered) | post-audit-P2-phase-0 | P3 | done | Matrix + gaps updated 2026-05-28; plan doc added |
| TD-073 | CI release gate for P0/P1 agentic-risk matrix rows | post-audit-P2-phase-1 | P1 | done | `Scripts/ci-llm-agentic-risk.sh`, `agentic-risk/manifest.json`, wired in `ci-llm-linux.sh` |
| TD-074 | Adversarial fixtures for indirect injection via tool/retrieval output | post-audit-P2-phase-2 | P1 | done | `ULLMTrustBoundary`, `Test_LLM_AdversarialFixtures`, `adversarial/*.json` |
| TD-075 | GUI consume `LLMFinalResponse.rollback_status` | post-audit-P2-phase-3 | P2 | done | `ULlmAssistantDockWidget` rollback status messages in stream + plan rollback |
| TD-076 | Provider HTTP retry honors `Retry-After` header | post-audit-P2-phase-4 | P2 | done | `ULLMHttpRetry`, `retry_after` on `ULLMHttpClient::Response`, `Test_LLM_HttpRetry` |
| TD-077 | Ambiguity gate blocks write until intent unambiguous | post-audit-P2-phase-5 | P2 | done | `ULLMIntentAmbiguityGate`, orchestrator block + `Test_LLM_IntentAmbiguityGate` |
| TD-078 | Windows Credential Manager for provider API keys | post-audit-P2-phase-6 | P2 | done | `CredReadW`/`CredWriteW` in `ULlmQtProviderSettingsSource` (manual Win verify) |
| TD-079 | Agentic SLO audit events (task success / false-execution / escalation) | post-audit-P2-phase-7 | P3 | done | `task_*`, `false_execution_prevented`, `escalation_to_hitl`; `Scripts/llm-audit-slo-snapshot.sh` |
| TD-080 | GUI chat history archive under `Bin/AiChats` + resume from JSON | chat-history | P2 | done | `ULlmChatHistoryArchive`, `ULlmChatHistoryDialog`, dock integration, `tryResumeSession`, `Test_LLM_ChatHistoryArchive` |
| TD-081 | Assistant read-only access to system logs + policy awareness | system-log-access | P2 | done | `UReadOnlyLogTail`, `ULLMSystemLogReader`, observability tools, orchestrator `system_log_excerpt` |
| TD-082 | Agent scenario matrix (JSON fixtures + deterministic CI + optional e2e) | agent-scenario-matrix | P1 | done | `Test_LLM_AgentScenarios`, `ci-llm-agent-scenarios.sh`, `Agent-Scenario-Matrix.md` |
| TD-049 | Extract `ULLMNameResolution` from `ULLMWriteArgumentNormalizer` | phase-A | P0 | done | Closed in phase-A commit: dedicated module + migrated callers |
| TD-050 | Tiered `URdkEntityResolver` (exact → CI → fuzzy) | phase-A | P0 | done | Closed in phase-A commit: centralized tiered resolver |
| TD-051 | Generalized pending disambiguation (not only class) | phase-B | P0 | done | `PendingToolArguments` now carries kind/field/candidates for class/component |
| TD-052 | Gateway pre-normalize all write entity-resolution tools | phase-B | P0 | done | Generic pre-normalize path for write tools with entity resolution |
| TD-053 | `connect_components` two-endpoint pending (from → to) | phase-B | P1 | done | Sequential resolution via `from_long_name` then `to_long_name` |
| TD-054 | `property_name` validation / `list_component_properties` read tool | phase-C | P1 | done | `get_component_properties` returns pub-parameter summary + optional values; `set_property` validates property_name |
| TD-055 | `get_net_snapshot.root_long_name` in domain | phase-C | P2 | done | Optional `root_long_name` in tool schema + `URdkDomainAccess::listNetSnapshot` subtree walk |
| TD-056 | `list_registered_classes.library_filter` schema drift | phase-C | P2 | done | `library_filter` + optional `library` via `UStorage::FindCollection` / `ULibrary::GetName` |
| TD-057 | Unified recent list dedupe rules | phase-D | P1 | done | QSettings + LastProjectsList merge implemented in `URdkApplicationCommands::listRecentConfigurations` |
| TD-058 | Recent configurations provider in presentation sink | phase-D | P1 | done | Sink API `ILLMPresentationSink::recentConfigurationPaths` + Qt impl `ULlmQtPresentationSink::recentConfigurationPaths` |
| TD-059 | `show_ui_panel` / `LLMUiPanel` presentation API | phase-E | P1 | done | Implemented via `LLMPresentationEvent.show_panel` + `UGEngineControlWidget::showLlmUiPanel` + tools `show_ui_panel` / `list_ui_panels` |
| TD-060 | `set_active_channel` GUI API research | phase-E | P3 | done | `set_active_channel` + `list_channels` via `Core_SelectChannel` and presentation sink GUI sync |
| TD-061 | `open_component_gui_tab` host dialog/automation gap | phase-E | P2 | done | Implemented tool `open_component_gui_tab` → `UGEngineControlWidget::promptAndOpenComponentGuiTabHost` (still user-assisted) |
| TD-062 | Task planner + `success_criteria` / `goal_success` for imperative multi-step mutate | post-MVP-agent | P1 | done | Completed in post-MVP autonomous planner implementation (PR4) |
| TD-063 | EN query normalization (LLM translate) before intent/planning | post-MVP-agent | P1 | done | Completed via `ULLMQueryNormalizer` + orchestrator wiring (PR1) |
| TD-064 | LinkPatternCatalog from Bin/Configs for connect inference | post-MVP-agent | P1 | done | Completed via link pattern build/index + connect normalizer integration (PR2+PR3) |
| TD-065 | Task executor verify-loop; remove harmful add_component early return | post-MVP-agent | P1 | done | Completed via task-path gating + verifier/retry/replan task executor (PR0+PR5) |
| TD-066 | Confidence-gated plan confirmation (complexity-dependent) | post-MVP-agent | P2 | done | Completed in `ULLMPlanConfidence` + task-path confirmation branch (PR4) |
| TD-067 | Lifecycle create-config NL parsing misses `parent_directory` in E2E scenarios | post-MVP-agent | P1 | done | Fixed via lifecycle arg merge/retry + create schema/path handling; `Test_LLM_E2eScenarios` now passes |
| TD-068 | Add strict rollback outcome status taxonomy in UI contract | phase-PR1 | P1 | done | Added explicit `LLMFinalResponse.rollback_status` and rollback status propagation (`rolled_back`, `partial_rollback`, `rollback_failed`, `rolled_back_nothing_to_compensate`) |
| TD-069 | Harden idempotency key derivation across retries with explicit step identity | phase-PR1 | P1 | done | Idempotency key derivation now uses SHA-256 and includes action identity (`tool_call_id` / confirmation id / retry marker) |
| TD-070 | Replace plaintext provider API key storage with OS-backed secure store | phase-PR2 | P0 | done | Added secure-store read/write in Qt provider settings source (`secret-tool` Linux / `security` macOS); plaintext fallback only via explicit env override |
| TD-071 | Add deterministic multi-session cancel isolation test fixture | phase-PR3 | P1 | done | Added `Test_LLM_Orchestrator.CancelSessionDoesNotAffectOtherSessions` |
| TD-041 | Manual NeuroModeler GUI write-tools walkthrough (real Configs on disk) | post-MVP | P3 | done | Covered via automated walkthrough simulation test (`Test_LLM_TD041WalkthroughSim`) for HITL reject + auto-apply retry; optional manual GUI spot-check remains |
| TD-036 | Embedding-based dynamic tool routing | post-MVP | P3 | done | Env-gated routing now supports lexical + score-based top-K subset via `ULLMDynamicToolRouter` (`NMSDK_LLM_DYNAMIC_TOOL_ROUTING_TOP_K`) |
| TD-031 | YAML knowledge manifest + federation | post-MVP | P3 | done | `UYamlKnowledgeCatalog` implemented and federated into `NmsdkBuiltinKnowledgeCatalog` (env/file-driven) |
| TD-034 | Function-level source chunking (ctags) | post-MVP | P3 | done | Implemented `UDocCtagsChunker` and source function-level indexing in `UDocSearchIndex` (scope=`sources`) |
| TD-083 | Context compaction + stale tool masking + `.full.jsonl` sidecar | context-P2 | P1 | done | `ULLMContextCompactor`, env `NMSDK_LLM_CONTEXT_COMPACT` |
| TD-084 | `Conversation-State.md` claims persisted session flags not in JSON | context-P0 | P1 | done | Docs synced with store v2 fields |
| TD-085 | LLM-based session summarize for compactor (v1.1) | context-P2 | P2 | done | `NMSDK_LLM_CONTEXT_COMPACT_LLM=1` + provider round; rule-based fallback |
| TD-086 | `diagram_viewport_scene_rect` in GUI context hints | context-P3 | P3 | cancelled | Post-MVP per context plan §8; not in `LLMGuiContextSnapshot` |
| TD-087 | `turn_id` per message in conversation store | post-context-plan | P3 | cancelled | Post-MVP ADR; store v2 has no per-message ids |
| TD-088 | Persist `LLMSessionContext` snapshot in session JSON (resume parity) | post-context-plan | P2 | done | `last_session_context` in store v2; GUI `buildSession` merge on resume |
| TD-089 | `Orchestrator.md` / `LLMRequestEnvelope` doc drift (gui_context, retriever) | context-P0 | P1 | done | Docs + `LLMGuiContextSnapshot` in envelope |
| TD-090 | Flaky lab Ollama e2e after context-hint changes | context-verify | P2 | done | Headless acceptable-failure in `llm_e2e_analyzer`; e2e fixture prompts updated |
| TD-091 | Connect planning (explicit pairs, N links, remaining/analogous) | connect-planner | P1 | done | `b41df580` (+ `4bf1a2b5`…`1ad30309`): snapshot `links[]`, session graph, deterministic planner, LLM fallback; `Docs/Link-Planning.md` |
| TD-092 | Unified turn controller owns full `handleUserMessage` body | phase-B | P1 | cancelled | Delegate + `handleUserMessageImpl` sufficient; full file split not required |
| TD-093 | Gateway `needs_clarification` → in-loop `ask_user` for all write tools | phase-C | P1 | done | `routeClarificationOrDisambiguation`, `NMSDK_LLM_CLARIFY_IN_LOOP`, `Test_LLM_AskUserProtocol` |
| TD-094 | LLM JSON input classifier calibration (RU/EN) | phase-D | P2 | done | `understandUserInput` LLM JSON + `Test_LLM_InputUnderstanding` |
| TD-095 | Failure corpus → live orchestrator regression harness | phase-A | P1 | done | `Test_LLM_FreeDialogueCorpus.MockOrchestratorHandlesCorpusPrompts` |
| TD-096 | Subagent explore: real isolated ReAct loop | phase-H | P2 | done | `ULLMSubagentRunner` read-only multi-round + `Test_LLM_SubagentRunner` |
| TD-097 | Embedding `search_tools` index versioning | phase-I | P2 | done | `ULLMSearchTools` lexical rank + `index_version`; `Test_LLM_SearchTools` |
| TD-098 | Settings UI for `task_path_mode` | phase-B | P2 | done | QSettings `LLM/task_path_mode` + provider settings widget |
| TD-099 | Default 14B migration: lab/embedded profiles | phase-M | P1 | done | `ULLMProviderCatalog` ollama defaults → qwen2.5:14b |
| TD-100 | Model benchmark CI tier gate | phase-M | P2 | done | `Test_LLM_ModelRouter` + manifest `free-dialogue-corpus` in CI via ctest |
| TD-101 | Lite tier router profile for weak GPUs | phase-M | P3 | done | `ollama-lite` profile + `routeModelForPhase(Router)` |
| TD-102 | Soft lifecycle: no default forced `tool_choice`; gate pre-LLM direct | phase-J | P1 | done | `lifecycleDirectInvokeEnabled`, `shouldForceLifecycleToolChoice`, `Test_LLM_LifecycleSoftAndTerminal` |
| TD-103 | `TurnTerminal` on `LLMFinalResponse` + cancel synthetic `tool_result` | phase-K | P1 | done | `TurnTerminal` enum, orchestrator assign + cancel audit |
| TD-104 | `propose_plan` orchestrator handler (not registry stub only) | phase-F | P1 | done | `executionPlanFromProposePlanArguments` + HITL pending_plan |
| TD-105 | Wire `ULLMModelRouter` before provider rounds in orchestrator | phase-M | P2 | done | `model_route_selected` audit + `model_override` for Router tier classifier |
| TD-106 | Embedding `ULLMEmbeddingToolRouter` + `tools-embeddings.jsonl` | phase-I | P2 | done | `searchToolsHybrid`, `LLM/index/tools-embeddings.jsonl` manifest |
| TD-107 | Agent scenario eval: `phase_entered`, `tool_loop_entered` audit expects | phase-G | P2 | done | `llm_agent_scenario_types` + `av2_tool_loop_audit` fixture |
| TD-108 | E2E `e2e_no_tool_email` flaky on live Ollama phrasing | post-M | P2 | done | Relaxed `NoSuitableToolOrRefusal` analyzer + headless/no-project acceptance |
| TD-109 | `Test_LLM_WriteToolsEngine` SIGSEGV on `UAppCore` exit | post-M | P2 | done | `ShutdownLlmWriteToolsEngine()` + gtest global env teardown |
| TD-110 | Symbol search (`UApplication`) rank below generic hits | post-M | P2 | done | Path/title boost in `UDocSearchIndex::searchInternal` |

---

## Done (archive)

| ID | Item | Resolved in | Resolution |
|----|------|-------------|------------|
| TD-119 | Manual GUI walkthrough: selection + connect after agent-quality | 2026-05-29 | Automated matrix + [`Chat-22-15-GUI-Walkthrough.md`](Docs/Chat-22-15-GUI-Walkthrough.md) for optional human sign-off |
| TD-128 | Dialog slot merge false connect on «add X to the model» | 2026-05-29 | `d4f631f7`: pair_re guard, disconnect routing, pre-gateway audit |
| TD-129 | Validate EN phrasing + observability tools in Query filter | 2026-05-29 | `f94a7dec`: `isValidateConfigurationGoalText`, `get_system_log_policy` in filter |
| TD-130 | GUI `context_acquisition_mode` + Mutate doc prefetch in Auto | 2026-05-29 | QSettings `LLM/context_acquisition_mode`; `Chat-22-15-GUI-Walkthrough.md` |
| TD-131 | Exact class in user text + post-verify resolve under parent | 2026-05-30 | `findExplicitRegisteredClassInUserText`, `resolveComponentLongName` |
| TD-132 | Open recent config without LLM prose / fix std::exception on open_recent | 2026-05-30 | `tryBuildOpenRecentInvokeArguments`, safe `open_recent_configuration` args |
| TD-133 | Direct add_component + safe tool args (no json::at throw) | 2026-05-30 | `tryPrepareAddComponentDirect`, gateway catch, `requireStringField` |
| TD-134 | Turn tool trace in chat + audit arguments_preview | 2026-05-30 | `TurnToolInvocationView`, gateway collector, GUI `<details>` |
| TD-135 | GuiTurnPin + resolveValidAddParent for add scope | 2026-05-30 | `active_turn_pin`, `ULLMAddParentResolution` |
| TD-136 | Entity path canonicalize + presentation scope guard | 2026-05-30 | `x-llm-semantic`, `ULLMPresentationScopeGuard`, `pin_diagram_for_writes` |
| TD-137 | Post-verify add uses gateway long_name only | 2026-05-30 | `verifyAddComponentEffect` early return on `long_name` |
| TD-138 | Migrate `set_property` / `remove_component` / link writes off `MModel_*` to engine-lock + `GetComponentL` | post-model-firmata | P2 | deferred | Indefinite (2026-07-27): umbrella Track 1 shelved. `add_component` done; remaining tools pass full `long_name` to `MModel_*`. Revisit triggers: write/lock/diagram bug, product demand to drop `MModel_*`, or dedicated sprint after GUI Track 2. |
| TD-139 | Never call `refreshDiagramPresentation` while holding engine/model lock | post-model-firmata | P1 | done | Scoped lock in `addComponent`; `DiagramRefresh` uses `QueuedConnection`; diagram list via `childComponentShortNamesFromModelScope` |
| TD-140 | Thinking-first: Ollama `think` + `message.thinking` in providers/types | thinking-A | P0 | done | Types, native/compat parse, history preserve, Test_LLM_Thinking |
| TD-141 | Progressive disclosure: `search_tools` expands allowlist for next rounds | thinking-B | P0 | done | enrichSearchToolsPayload + expandToolFilterFromSearchResult |
| TD-142 | Wire Cortex thinking profile + no forced tool_choice when think ON | thinking-C | P0 | done | ollama-thinking default, enable_ollama_thinking, orchestrator wiring |
| TD-143 | Collapsible GUI thinking bubble / on_thinking_token UI | thinking-post | P2 | done | `757efc80`: Dock `onThinkingToken` + `<details>Reasoning</details>`; status “Model is reasoning…” |
| TD-147 | Defer LLM doc-index rebuild off UI-thread startup (progress ~20% hang) | startup | P1 | done | Gate on `ShowLlmAssistantMenu` / `NMSDK_LLM_*`; async `initialize`; stale prebuilt; sync via `NMSDK_LLM_INDEX_SYNC_ON_START=1` |
| TD-148 | Opaque GUI `Error: std::exception` on query (Qt5 Concurrent + qwen3 `reasoning`) | thinking-fix | P0 | done | Worker/orchestrator catch + audit `turn_exception`; JSON dump `error_handler_t::replace`; parse OpenAI-compat `reasoning` |
| TD-149 | Query context for current model + chat markdown render | context-chat-md | P1 | done | Query/Explain decision tree; live net snapshot when project_loaded; router prefer snapshot; `ULlmChatMarkdown` via QTextDocument |
| TD-150 | RecordedToolInvoke: memory parity for pre-LLM / FastPath / Plan / direct-HITL | memory-parity | P1 | done | `ULLMRecordedToolInvoke`; HITL `tool_call_id`; Task/Plan options; docs DD-MEM-001/002 |
| TD-151 | Connect inspect + live analogous replicate (subtree filters, ports tool) | connect-inspect | P1 | done | `list_model_links` subtree filters + allowlist; `get_component_ports`; `analogous_ref_token` fan-out; DD-CONN-001 |
| TD-152 | Connect funnel: intent `подключ`, autonomous inspect, session peers, HintOnly FastPath | connect-inspect | P1 | done | Intent force Mutate; whitelist; `wants_session_peers`; live-analogous execute under HintOnly |
| TD-153 | Act-or-Clarify post-think contract (no free essay on actionable goals) | act-or-clarify | P0 | done | `ULLMActOrClarifyGate`; recovery independent of misclassified Query; DD-ACT-001 |
| TD-154 | Channel calc FastPath + autonomous whitelist (`ask_user`, calc tools) | calc-route | P0 | done | `ULLMChannelCalcCommand`; DD-CALC-001; Act-or-Clarify calc recovery |
| TD-144 | Parallel independent tool fan-out in one assistant turn | thinking-post | P3 | deferred | Sequential tools OK for local ReAct MVP |
| TD-145 | Ollama think levels (`low`/`medium`/`high`) / gpt-oss | thinking-post | P3 | deferred | Boolean `think` sufficient for qwen3:14b/deepseek-r1 |
| TD-146 | Embed-index quality for `search_tools` hybrid ranking | thinking-post | P2 | deferred | Lexical+existing embeddings; revisit if discovery miss-rate high |
| TD-120 | Remove hardcoded connect paths in Core ephemeral/heuristics | 2026-05-29 | `f3f4cc0b`: KnowledgeBlocks, acquisition policy, no LTZone in Core |
| TD-121 | Context acquisition policy + catalog blocks in ephemeral | 2026-05-29 | `f3f4cc0b`: `computeContextAcquisitionPlan`, retriever diagram scope |
| TD-122 | diagram_scope_long_name GUI + add parent parity | 2026-05-29 | `3c5b615f`: diagramScopeChanged, normalizer parent |
| TD-123 | ULLMDialogSlotMerge multi-turn connect/lifecycle | 2026-05-29 | `ULLMDialogSlotMerge`, orchestrator pending hook |
| TD-124 | ActionIntentPreview + unified write execution | 2026-05-29 | `ULLMActionIntentPreview`, `executeWriteWithPreviewAndVerify` |
| TD-125 | Post-tool verify + bounded repair loop | 2026-05-29 | `runPostToolVerification`, `attemptRepair`, max 2 repairs |
| TD-126 | Mutate-turn integrity (no fake success prose) | 2026-05-29 | embedded validate, connect recovery partial JSON |
| TD-127 | HTML tool rows + known_facts on connect | 2026-05-29 | `recordSessionConnect`, known_facts persist load |
| TD-111 | GUI selection → `Env_SelectCurrentComponent` | 2026-05-29 | **cancelled/superseded 2026-07-29**: leaf/drill→Env mixed FindComponent cursor with GUI focus; broke Del/props/links. Reverted sync call sites; snapshot uses diagram_scope/focused |
| TD-112 | `LLMGuiContextSnapshot` current_component_* from Env | 2026-05-29 | **superseded 2026-07-29**: `guiSnapshotFromContext` fills from diagram_scope/focused, not Env |
| TD-113 | Write tools default scope = CurrentComponent | 2026-05-29 | superseded by TD-122 `diagram_scope_long_name` + `fillAddComponentDefaults` |
| TD-114 | Entity resolve under current scope; connect vs add class | 2026-05-29 | `snapshotComponentsUnderScope`, `WRONG_TOOL_FOR_CONNECT` |
| TD-115 | Connect recovery round + narrowed tool filter | 2026-05-29 | orchestrator `connect_recovery_round` |
| TD-116 | RU connect pair regex (`к`, `→`) | 2026-05-29 | `ULLMConnectPlanParsing` pair_re |
| TD-117 | Response language in manifest | 2026-05-29 | `buildAgentManifest(..., response_language)` |
| TD-118 | `FindComponent("")` = CurrentComponent (docs) | 2026-05-29 | DD-AG-001: empty = Current (usually Model in GUI). Do not sync selection→Env |
| TD-093 | Snapshot link walk + list_model_links | 2026-05-29 | `7e7ac67f`: `ULLMModelLinkWalker`, `list_model_links`, `linkExistsInModel` truncated dedup |
| TD-096 | Connect semantics index + runtime inference | 2026-05-29 | `8641b0a3`: `connect-semantics.json`, catalog, port inference, context hint |
| TD-092 | Global remaining + chain/tree pairing | 2026-05-29 | `b41e7082`: `ULLMConnectEndpoints`, `ULLMConnectPairing`, ModelGraph, topologies |
| TD-091 | Connect planning (explicit pairs, N links, remaining/analogous) | 2026-05-28 | `b41df580`: deterministic connect planner, session graph, snapshot links, idempotent connect |
| TD-019 | MotionControlLib `Llm/` read tools | 2026-05-25 | `RegisterMotionControlLibLlmTools` |
| TD-020 | Plan rollback for `set_property` | 2026-05-25 | `previous_value` + compensation in `ULLMPlanExecutor` |
| TD-021 | Ollama `/api/embeddings` doc re-rank | 2026-05-25 | `UDocOllamaEmbeddings`, `NMSDK_LLM_DOC_EMBED_OLLAMA=1` |
| TD-022 | GUI plan preview dock wired | 2026-05-25 | `LlmGui::showPlanPreview` from assistant dock |
| TD-023 | Plan checkpoints / resume mid-plan | 2026-05-25 | `plan_checkpoint`, `resumePlanExecution`, GUI Resume/Rollback |
| TD-026 | `connect_components` + project load/save tools | 2026-05-25 | `RegisterCoreRdkTools`, `URdkDomainAccess` |
| TD-029 | Ollama remote model mismatch hints + env defaults | 2026-05-25 | `NMSDK_LLM_OLLAMA_*`, `/api/tags`, healthCheck |
| TD-028 | Strict json_schema execution plan (cloud) | 2026-05-25 | `executionPlanOpenAiResponseFormat`, Plan intent |
| TD-027 | L7 regression fixture runner | 2026-05-25 | `Test_LLM_RegressionFixtures`, `regression/*.json` |
| TD-025 | HITL confirmation TTL auto-expire | 2026-05-25 | `created_at_unix_sec`, persist `pending`, GUI timer |
| TD-024 | GUI token streaming (`chatStream`) | 2026-05-25 | SSE in `ULLMHttpClient`, `LLMStreamHandlers`, Cancel button |
| TD-017 | Doc search vector / semantic ranking | 2026-05-25 | Random-projection hybrid in `UDocSearchIndex` |
| TD-018 | Plan saga auto-undo add_component | 2026-05-25 | `remove_component` on failed plan |
| TD-002 | Intent classification (rules + optional LLM) | 2026-05-25 | `parseDetailed`, `NMSDK_LLM_INTENT_LLM=1` |
| TD-003 | Doc search TF-IDF index | 2026-05-26 | `UDocSearchIndex`, `searchDocsWithIndex`, `buildFromCatalog`, scope |
| TD-004 | Scenario C plan execute workflow | 2026-05-25 | `ULLMPlanExecutor`, GUI Run plan, policy `checkPlan` |
| TD-015 | Ollama `/api/show` modelfile → chat template family | 2026-05-25 | `UOllamaModelInfo`, `resolveChatTemplateFamily` |
| TD-016 | Audit sanitization + schema regression tests | 2026-05-25 | `ULLMAuditSanitizer`, `ULLMToolSchemaRegression`, CI via ctest |
| TD-014 | Phase 4: `UEmbeddedLlamaProvider` + llama.cpp | 2026-05-25 | `ULlamaRuntime`, `rdk.llm.embedded`, `Scripts/ci-llm-embedded-linux.sh` |
| TD-007 | `UOllamaNativeProvider` fallback | 2026-05-25 | `UOllamaNativeProvider`, profile `ollama-native` |
| TD-009 | Library Llm `HardwareLib/Llm` | 2026-05-25 | `RegisterHardwareLibLlmTools`, NeuroModeler wiring |
| TD-001 | Apply `RDK_USE_LLM` + vcpkg `llm` + `add_subdirectory(LLM)` | 2026-05-25 | `cmake/RdkDefines.cmake`, `Rdk/CMakeLists.txt`, `vcpkg.json` |
| TD-011 | `set_property` via `MModel_SetComponentPropertyValue` | 2026-05-25 | `URdkDomainAccess::setProperty` |
| TD-012 | Provider settings UI + QSettings/env API key resolution | 2026-05-25 | `ULLMProviderAuth`, `ULlmProviderSettingsWidget` |
| TD-005 | RBAC guest deny write (`user_id < 0`) | 2026-05-25 | `ULLMPolicyEngine` P10 |
| TD-008 | Session persist JSON under `<repo>/LLM/sessions/` | 2026-05-25 | `ULLMConversationStore` |
| TD-010 | `llm_audit_verify` CLI + `verifyJsonlFile` | 2026-05-25 | `Rdk/LLM/Tools/llm_audit_verify_main.cpp` |
| TD-013 | Entity resolver + ambiguous clarification UI | 2026-05-25 | `URdkEntityResolver`, orchestrator |
| TD-006 | Parallel read tools in orchestrator round | 2026-05-25 | `std::async` when all Read |
| TD-037 | Multilingual retrieval boost (RU query) | 2026-05-26 | UTF-8 token bytes preserved in `UDocSearchIndex::tokenize` |
| TD-040 | Catalog fingerprint includes source mtimes | 2026-05-26 | `last_write_time` per catalog root in `catalogFingerprint()` |
| TD-033 | Incremental index / mtime watcher | 2026-05-26 | `syncFromCatalog`, `file_mtimes` in manifest, dev cache `LLM/index` |
| TD-030 | Write-tools E2E hardening (automated) | 2026-05-26 | `Test_LLM_WriteToolsAudit` (mock → HITL), lifecycle mock/lab; `tool_invoke_*` via lab/GUI |
| TD-032 | Auto-generate `Docs/llms.txt` from catalog | 2026-05-26 | `NmsdkBuiltinKnowledgeCatalog::writeLlmsTxt` via `llm-index-pack` |
| TD-039 | `loadedLibraries()` from catalog | 2026-05-26 | `NmsdkBuiltinKnowledgeCatalog::loadedLibraries()`; context delegates |
| TD-031 | YAML knowledge manifest + federation | 2026-05-26 | Cancelled MVP: C++ catalog; revisit TD-031 if YAML needed |
| TD-034 | Function-level source chunking | 2026-05-26 | Cancelled MVP: 120-line file excerpts; ctags post-MVP |
| TD-036 | Embedding-based dynamic tool routing | 2026-05-26 | Cancelled MVP: static `ULLMToolFilterBuilder` |
| TD-043 | Write-tools P1 backlog (policy, add_component, tests) | 2026-05-26 | `ULLMWriteToolPolicy`, `Test_LLM_WriteToolsP1`, `Test_LLM_WriteToolsEngine`, domain writes |
| TD-042 | Write-tools P1 on live engine | 2026-05-26 | `Test_LLM_WriteToolsEngine` + `test_valid` via `LlmWriteToolsEngineInit` |
| TD-044 | Write-tools P2 integration | 2026-05-26 | Library write tools, `ULLMWriteArgumentNormalizer`, `PLAN_NEEDS_SNAPSHOT`, `Test_LLM_WriteToolsP2` |
| TD-046 | GUI new chat + session rotation on project load | 2026-05-26 | `discardSession`, unique `gui-*` session_id, Ollama `/api/tags` in Settings |
| TD-045 | Embedded provider live token stream in GUI | 2026-05-26 | `ULlamaRuntime::completeStream`, `supports_streaming`, GUI streams EmbeddedLlama |
| TD-047 | Docs bundle 1.4 sync | 2026-05-25 | Developer-Architecture, Extension-Guide, Post-MVP-Implementation-Plan; README 1.4.0 |
| TD-048 | Scenario D autonomous agent (strict / semi_auto) | 2026-05-25 | `ULLMAutonomousPolicy`, settings/GUI, orchestrator whitelist+step cap, `Test_LLM_AutonomousPolicy` |
| TD-035 | Dedicated `tool_disambiguation` UI flag | 2026-05-25 | `AgentResponse::needs_tool_disambiguation`; dock clarification block |
| TD-049 | Extract `ULLMNameResolution` from `ULLMWriteArgumentNormalizer` | 2026-05-26 | `ULLMNameResolution.{h,cpp}` + normalizer/lifecycle integration |
| TD-050 | Tiered `URdkEntityResolver` (exact → CI → fuzzy) | 2026-05-26 | `resolveComponentEntity` + `URdkEntityResolver` delegation |
| TD-051 | Generalized pending disambiguation (not only class) | 2026-05-26 | `PendingToolArguments` + orchestrator/gate generalized disambiguation flow |
| TD-052 | Gateway pre-normalize all write entity-resolution tools | 2026-05-26 | `ULLMToolGateway` generic pre-normalize before schema/policy |
| TD-053 | `connect_components` two-endpoint pending (from → to) | 2026-05-26 | Field-aware pending with `from_long_name`/`to_long_name` picks |

---

## Deferred decisions log (ADR-style, brief)

| Date | Decision | Rationale | Revisit_by_phase | Revisit_trigger |
|------|----------|-----------|------------------|----------------|
| 2026-05-25 | Single `TECH-DEBT.md` at `Rdk/LLM/` root | Visible next to code; linked from AGENTS.md | — | — |
| 2026-05-25 | One English commit per implementation phase | Reviewable history, bisect-friendly | — | — |
| 2026-05-25 | Builtin C++ catalog vs YAML | Faster MVP, no yaml-cpp dependency | post-MVP | TD-031 |
| 2026-05-25 | Remove pre-LLM lifecycle bypass | LLM-first agent model | — | — |
| 2026-05-25 | Ship index in `Bin/LLM/index` | Prod without full source tree | post-MVP | TD-033 |
| 2026-05-25 | `Rdk/LLM` must not hardcode NMSDK `Bin/` paths | Core vs product separation | — | — |
| 2026-05-25 | Offline projection vs full embedding model | No GGUF embed model in MVP; hybrid TF-IDF sufficient | — | TD-021 (Ollama optional) |
| 2026-05-25 | Ollama embed re-rank off by default | Avoid CI/network dependency | — | `NMSDK_LLM_DOC_EMBED_OLLAMA=1` |
| 2026-05-26 | Manual GUI write-tools split to TD-041 | Automated tests cover LLM-first path | post-MVP | TD-041 |
| 2026-05-25 | Post-MVP code in separate plan doc | Implementation tracked in Post-MVP-Implementation-Plan.md | post-MVP | TD-048, TD-035, TD-031/034/036 |
| 2026-05-26 | Consolidate name/entity resolution in domain helper | Reuse same disambiguation behavior across tools before pending generalization | — | TD-051/052 |
| 2026-05-26 | Keep direct gateway calls backward-compatible without user hint pre-normalization | Unit tests and non-orchestrator callers expect confirmation-first behavior | post-MVP | after orchestration-only invoke contract exists |
| 2026-05-26 | Deliver read-path class disambiguation first in phase C | `describe_class` now resolves fuzzy/CI names; schema gaps (`root_long_name`, `library_filter`) deferred explicitly | — | TD-054/055/056 |
| 2026-05-28 | DD-CTX-001: Ephemeral hints via `ULLMContextAssembler`, not stored in `messages` (except bootstrap/summary) | Avoid persist bloat; match current manifest behavior | — | — |
| 2026-05-28 | DD-CTX-002: Compactor default = rule-based; optional LLM summarize | Deterministic CI default; cost when `NMSDK_LLM_CONTEXT_COMPACT_LLM=1` | — | TD-085 done |
| 2026-05-28 | DD-CTX-003: Query prefetch on non-empty `retrieval_query`; Mutate prefetch in **Auto** mode (+ env override) | Avoid duplicate with `search_project_docs` tool | chat-22-15 tails | GUI `LLM/context_acquisition_mode`; `Minimal` skips mutate prefetch |
| 2026-05-28 | DD-CTX-004: Retriever when list focus **or** `diagram_scope_long_name` (not focus-only) | Token/latency; drill-level context without list selection | chat-22-15-PR1 | Implemented 2026-05-29 (`allow_retriever_without_list_focus`) |
| 2026-05-28 | DD-CTX-005: Single `TECH-DEBT.md` for all context work (no second file) | Repo convention since 2026-05-25 | — | — |
| 2026-05-29 | DD-UT-001: Task path default HintOnly; FastPath CI-only | Agent-first; avoid hard planner fail | — | `NMSDK_LLM_TASK_PATH_STRICT` |
| 2026-05-29 | DD-UT-002: `ask_user` vs legacy disambiguation coexist | Gradual GUI migration | phase-C | TD-093 |
| 2026-05-29 | DD-UT-003: Subagent runner stub until budget policy | Phase H scope | phase-H | TD-096 |
| 2026-07-27 | Shelve umbrella Track 1 (LLM production hardening) indefinitely | MVP+ write-tools/agent v2/post-audit complete; TD-138 not MVP-blocking; prioritize GUI/docs | — | TD-138: reproducible write/lock/diagram bug; product demand to drop `MModel_*`; dedicated sprint after GUI Track 2 |
| 2026-07-30 | DD-THINK-001: Cortex thinking-first via Ollama native `/api/chat` (`think:true`) | `/v1` historically drops `think` on some Ollama builds; native is source of truth | thinking-C | Lab /v1 parity verified |
| 2026-07-30 | DD-THINK-002: Preserve unmodified `thinking` on assistant tool_call turns | Anthropic/Qwen/DeepSeek tool+thinking contract; mid-loop rewrite degrades multi-step | — | — |
| 2026-07-30 | DD-THINK-003: `tool_choice=auto` only when think ON (no lifecycle force) | Forced tool_choice conflicts with thinking models | — | — |
| 2026-07-30 | DD-THINK-004: Default Cortex profile `ollama-thinking` (**qwen3:14b**); Utility/Router think OFF | Thinking reserved for ReAct cortex; short structured calls stay cheap | thinking-C | Pin updated 2026-07-30 tails |
| 2026-07-30 | DD-THINK-005: Progressive disclosure via `search_tools` expand allowlist | Full registry every round hurts local tool selection; discovery must open schemas | thinking-B | TD-141 |
| 2026-07-31 | DD-MEM-001: Pre-LLM and plan/task invokes must use `RecordedToolInvoke` | Same memory contract as ReAct (paired tool_calls/results + session_graph); closes follow-up blindness | memory-parity | TD-150 |
| 2026-07-31 | DD-MEM-002: Always-on direct allowlist: explicit-class add, open_recent index, pending-arg resume; lifecycle **load** remains env-gated (TD-102) | Deterministic UX when args fully specified; load stays opt-in | — | Unified-Turn § Recorded bypass |
| 2026-07-31 | DD-MEM-003: Continuer nouns are not class queries; `last_add` is default for continuity follow-ups; resolved registered class_name must not be fuzzy-overwritten; CLASS_AMBIGUOUS surfaces last_add first | Fixes «ещё N нейрона/компонента» asking wrong class list | memory-parity | — |
| 2026-07-31 | DD-CONN-001: Connect names are subtree anchors; live analogous uses hub↔ref nested template links, not only `last_template` / root↔root | Log 22-08-46: replicate Gen→PNeuron wiring onto peers without hallucinated topology | connect-inspect | TD-151 |
| 2026-08-01 | DD-ACT-001: After think, actionable goals with tools offered must end in tool / ask_user / NO_SUITABLE_TOOL — not free prose | Log 10-35-14: think→essay when Query + empty tools; DD-THINK-003 keeps API tool_choice=auto | act-or-clarify | TD-153 |
| 2026-08-01 | DD-CONN-002: `подключ*` is Mutate; live-analogous ConnectPlan FastPath even under HintOnly; session «этим» peers | Closes funnel after TD-151 tools existed but intent/HintOnly blocked execute | connect-inspect | TD-152 |
| 2026-08-01 | DD-CALC-001: «запусти расчет» / start|pause|reset|step calculation → deterministic `*_channel_calculation` FastPath before TaskPath; autonomous allows calc tools + `ask_user` | Log 16-31-41: thinking essay after ask_user autonomous deny; tools already existed | calc-route | TD-154 |
