# MVP Roadmap

## RU

## Git and tech debt (each phase)

After each implementation phase, follow [Development-Workflow.md](Development-Workflow.md):

1. Review and update [TECH-DEBT.md](../TECH-DEBT.md) (close what you can; log new deferrals)
2. Create **one English commit** per phase (see workflow doc for message templates)
3. Tick criteria below when verified

---

## EN

## Product stages (из enterprise LLM integration practices)

### Stage 0 — Scaffold (phase 0)

- CMake `RDK_USE_LLM`, `rdk.llm.core` stubs, `add_subdirectory(LLM)`
- Close **TD-001** (Pending-CMake-Changes)

**Commit:** `build(rdk-llm): add optional RDK_USE_LLM target and core stubs`  
*(Docs-only milestone, if already committed separately:)* `docs(rdk-llm): add LLM design documentation package`

---

### Stage 1 — Chat over data (phase 1 implementation) ✅ MVP start

**Scenario A.** User asks о diagram, classes, documentation.

| Included | Excluded |
|----------|-----------|
| Read tools (все) | Write tools |
| `search_project_docs` | Plan/Execute batch |
| GUI chat + token streaming (OpenAI-compat) | Embedded live stream |
| Ollama + OpenAI-compat | Autonomous loop |
| Entity resolution read | Mass mutations |

**Readiness criterion:**
- [x] Answer «which components на diagram» с `get_net_snapshot`
- [x] «Что does NModel» с `describe_class` + doc snippet
- [x] Audit JSONL is written
- [x] `RDK_USE_LLM=OFF` build без errors

**Commit:** `feat(rdk-llm): read-only tools, orchestrator, and assistant dock`

---

### Stage 2 — Copilot for actions (phase 2)

**Scenario B.**

| Included | Excluded |
|----------|-----------|
| Write tools + HITL | Scenario D autonomous |
| Plan preview + execute | Parallel write |
| `add_component`, `set_property`, `connect_components`, … | — |
| Idempotency keys | Parallel write |

**Readiness criterion:**
- [x] Add component на scheme after Confirm
- [x] Deny write без open project
- [x] Ambiguous name → clarification UI
- [x] Provider auth: QSettings + env + cloud policy (P09)

**Commit:** `feat(rdk-llm): write tools with policy, HITL, and NMSDK context`

---

### Stage 2b — Library extensions (optionally in parallel)

- [x] `Libraries/Rdk-HardwareLib/Llm/` — read tools + doc paths
- [x] `Libraries/Nmsdk-PulseLib/Llm/` — `search_pulse_docs`, `list_pulse_component_classes`
- [x] `Nmsdk-MotionControlLib/Llm/` — `search_motion_control_docs`, `list_motion_control_component_classes`

---

### Stage 3 — Hardening (phase 3)

- Full test pyramid
- Policy tests
- Session persist
- Hash chain verify
- Rate limits cloud

**Readiness criterion:**
- [x] Cloud session rate limit (`max_cloud_provider_rounds_per_session`)
- [x] L1–L5 unit tests (validator, policy, intent, gateway, orchestrator)
- [x] `Scripts/ci-llm-linux.sh` for CI matrix
- [x] Session persist (TD-008)
- [x] Hash chain verify CLI (TD-010)

**Commit:** `test(rdk-llm): add LLM unit tests, audit log, and CI matrix`

---

### Stage 4 — Embedded llama (mandatory по product)

- [x] `RDK_LLM_BUILD_EMBEDDED=ON` + `RDK_LLM_LLAMA_CPP_DIR` / FetchContent b4533
- [x] `UEmbeddedLlamaProvider` + `ULlamaRuntime` (`rdk.llm.embedded`)
- [x] GUI profile `embedded-offline` (Model = path to `.gguf`)
- [x] CI `Scripts/ci-llm-embedded-linux.sh`

**Commit:** `feat(rdk-llm): add embedded llama.cpp provider target`

---
