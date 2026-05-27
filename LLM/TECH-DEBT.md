# RDK LLM — Technical Debt Log

Living document. Update **after every phase** (see [Docs/Development-Workflow.md](Docs/Development-Workflow.md)).

**Rules:**
- Any deferred decision, shortcut, or known gap → add a row **before** phase commit.
- At phase end: try to close items marked `can_resolve_now`; re-prioritize the rest.
- Do not delete rows; set `Status` to `done` / `cancelled` with date.
- Any “we will decide later” must be captured either:
  - as a TD row (if it implies work), or
  - as an entry in **Deferred decisions log** (if it is an ADR-style choice to revisit).

---

## Status legend

| Status | Meaning |
|--------|---------|
| `open` | Not started |
| `in_progress` | Actively worked this phase |
| `done` | Resolved (note commit or PR in Resolution) |
| `cancelled` | No longer needed |
| `blocked` | External dependency |

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
| TD-041 | Manual NeuroModeler GUI write-tools walkthrough (real Configs on disk) | post-MVP | P3 | open | Checklist: Application-Commands.md § TD-041; incl. auto-apply + Reject flow |
| TD-036 | Embedding-based dynamic tool routing | post-MVP | P3 | open | Stub `ULLMDynamicToolRouter`; env `NMSDK_LLM_DYNAMIC_TOOL_ROUTING=1` |
| TD-031 | YAML knowledge manifest + federation | post-MVP | P3 | open | Stub `ILLMYamlKnowledgeCatalog`; Post-MVP plan §5 |
| TD-034 | Function-level source chunking (ctags) | post-MVP | P3 | open | Stub `UDocCtagsChunker`; Post-MVP plan §5 |

---

## Done (archive)

| ID | Item | Resolved in | Resolution |
|----|------|-------------|------------|
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
