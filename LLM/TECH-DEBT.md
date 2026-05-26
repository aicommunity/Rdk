# RDK LLM — Technical Debt Log

Living document. Update **after every phase** (see [Docs/Development-Workflow.md](Docs/Development-Workflow.md)).

**Rules:**
- Any deferred decision, shortcut, or known gap → add a row **before** phase commit.
- At phase end: try to close items marked `can_resolve_now`; re-prioritize the rest.
- Do not delete rows; set `Status` to `done` / `cancelled` with date.

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
| TD-041 | Manual NeuroModeler GUI write-tools walkthrough (real Configs on disk) | post-MVP | P3 | open | Optional QA; automated: TD-030, `Test_LLM_WriteToolsEngine`, Application-Commands.md § TD-041 |

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
| TD-035 | Dedicated `tool_disambiguation` UI flag | 2026-05-26 | Deferred: reuse `needs_entity_clarification` per LLM-first MVP |
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

---

## Deferred decisions log (ADR-style, brief)

| Date | Decision | Rationale | Revisit |
|------|----------|-----------|---------|
| 2026-05-25 | Single `TECH-DEBT.md` at `Rdk/LLM/` root | Visible next to code; linked from AGENTS.md | — |
| 2026-05-25 | One English commit per implementation phase | Reviewable history, bisect-friendly | — |
| 2026-05-25 | Builtin C++ catalog vs YAML | Faster MVP, no yaml-cpp dependency | TD-031 |
| 2026-05-25 | Remove pre-LLM lifecycle bypass | LLM-first agent model | — |
| 2026-05-25 | Ship index in `Bin/LLM/index` | Prod without full source tree | TD-033 |
| 2026-05-25 | `Rdk/LLM` must not hardcode NMSDK `Bin/` paths | Core vs product separation | — |
| 2026-05-25 | Offline projection vs full embedding model | No GGUF embed model in MVP; hybrid TF-IDF sufficient | TD-021 Ollama optional |
| 2026-05-25 | Ollama embed re-rank off by default | Avoid CI/network dependency | `NMSDK_LLM_DOC_EMBED_OLLAMA=1` |
| 2026-05-26 | Manual GUI write-tools split to TD-041 | Automated tests cover LLM-first path | TD-041 |
