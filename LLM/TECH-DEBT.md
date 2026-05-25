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

| ID | Item | Deferred in | Target phase | Priority | Status | Notes / resolution |
|----|------|-------------|--------------|----------|--------|-------------------|
| TD-002 | Intent parser: rule-based only; no LLM-based intent classification | design | post-MVP | P2 | open | Extended keywords (arduino, explain); LLM classifier still future |
| TD-003 | Doc search: no embeddings index | design | post-MVP | P3 | open | `UDocSearchHelper` multi-token ranking (2026-05-25) |
| TD-004 | Scenario C saga/compensation | design | post-MVP | P3 | open | Phase enum + audit transitions only; no saga yet |
| TD-014 | Phase 4: real llama.cpp in `UEmbeddedLlamaProvider` | design | 4 | P2 | open | Stub + `embedded-offline` profile in catalog |

---

## Done (archive)

| ID | Item | Resolved in | Resolution |
|----|------|-------------|------------|
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

---

## Deferred decisions log (ADR-style, brief)

| Date | Decision | Rationale | Revisit |
|------|----------|-----------|---------|
| 2026-05-25 | Single `TECH-DEBT.md` at `Rdk/LLM/` root | Visible next to code; linked from AGENTS.md | — |
| 2026-05-25 | One English commit per implementation phase | Reviewable history, bisect-friendly | — |
| 2026-05-25 | `Rdk/LLM` must not hardcode NMSDK `Bin/` paths | Core vs product separation | — |
