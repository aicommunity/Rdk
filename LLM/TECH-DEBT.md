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
| TD-011 | `set_property` tool: wire full UProperty API in `URdkDomainAccess` | phase 2 | 2 | P1 | open | Stub returns InvalidPropertyValue |
| TD-002 | Intent parser: rule-based only; no LLM-based intent classification | design | 3 | P2 | open | See Orchestrator.md |
| TD-003 | Doc search: keyword match only; no embeddings index | design | post-MVP | P3 | open | Project-Context-NMSDK.md |
| TD-004 | Scenario C workflow state machine — design only | design | post-MVP | P3 | open | Orchestrator.md |
| TD-005 | RBAC per `UApplication::GetUserId()` | design | 3 | P2 | open | Policy-and-Safety.md |
| TD-006 | Parallel read tool invocations in gateway | design | 3 | P2 | open | Tool-Gateway-and-Registry.md |
| TD-007 | `UOllamaNativeProvider` optional fallback | design | 2 | P3 | open | Providers.md |
| TD-008 | Session persist to disk (`UserConfig/LLM/sessions`) | design | 3 | P2 | open | Conversation-State.md |
| TD-009 | Library Llm folders (`HardwareLib/Llm`, …) | design | 2b | P2 | open | Project-Context-NMSDK.md |
| TD-010 | Hash-chain audit verify CLI | design | 3 | P2 | open | Observability-and-Audit.md |

---

## Done (archive)

| ID | Item | Resolved in | Resolution |
|----|------|-------------|------------|
| TD-001 | Apply `RDK_USE_LLM` + vcpkg `llm` + `add_subdirectory(LLM)` | phase 0 | 0 | P0 | done | `cmake/RdkDefines.cmake`, `Rdk/CMakeLists.txt`, `vcpkg.json` |

---

## Deferred decisions log (ADR-style, brief)

| Date | Decision | Rationale | Revisit |
|------|----------|-----------|---------|
| 2026-05-25 | Single `TECH-DEBT.md` at `Rdk/LLM/` root | Visible next to code; linked from AGENTS.md | — |
| 2026-05-25 | One English commit per implementation phase | Reviewable history, bisect-friendly | — |
| 2026-05-25 | `Rdk/LLM` must not hardcode NMSDK `Bin/` paths | Core vs product separation | — |
