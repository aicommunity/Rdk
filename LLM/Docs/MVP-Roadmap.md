# MVP Roadmap

## Git and tech debt (each phase)

After each implementation phase, follow [Development-Workflow.md](Development-Workflow.md):

1. Review and update [TECH-DEBT.md](../TECH-DEBT.md) (close what you can; log new deferrals)
2. Create **one English commit** per phase (see workflow doc for message templates)
3. Tick criteria below when verified

---

## Этапы продукта (из enterprise LLM integration practices)

### Этап 0 — Scaffold (фаза 0)

- CMake `RDK_USE_LLM`, `rdk.llm.core` stubs, `add_subdirectory(LLM)`
- Close **TD-001** (Pending-CMake-Changes)

**Commit:** `build(rdk-llm): add optional RDK_USE_LLM target and core stubs`  
*(Docs-only milestone, if already committed separately:)* `docs(rdk-llm): add LLM design documentation package`

---

### Этап 1 — Chat over data (фаза 1 реализации) ✅ MVP start

**Сценарий A.** Пользователь спрашивает о схеме, классах, документации.

| Включено | Исключено |
|----------|-----------|
| Read tools (все) | Write tools |
| `search_project_docs` | Plan/Execute batch |
| GUI chat + token streaming (OpenAI-compat) | Embedded live stream |
| Ollama + OpenAI-compat | Autonomous loop |
| Entity resolution read | Mass mutations |

**Критерий готовности:**
- [x] Ответ «какие компоненты на схеме» с `get_net_snapshot`
- [x] «Что делает NModel» с `describe_class` + doc snippet
- [x] Audit JSONL пишется
- [x] `RDK_USE_LLM=OFF` сборка без ошибок

**Commit:** `feat(rdk-llm): read-only tools, orchestrator, and assistant dock`

---

### Этап 2 — Copilot for actions (фаза 2)

**Сценарий B.**

| Включено | Исключено |
|----------|-----------|
| Write tools + HITL | Scenario D autonomous |
| Plan preview + execute | Parallel write |
| `add_component`, `set_property`, `connect_components`, … | — |
| Idempotency keys | Parallel write |

**Критерий готовности:**
- [x] Add component на схему после Confirm
- [x] Deny write без open project
- [x] Ambiguous name → clarification UI
- [x] Provider auth: QSettings + env + cloud policy (P09)

**Commit:** `feat(rdk-llm): write tools with policy, HITL, and NMSDK context`

---

### Этап 2b — Library extensions (опционально параллельно)

- [x] `Libraries/Rdk-HardwareLib/Llm/` — read tools + doc paths
- [x] `Libraries/Nmsdk-PulseLib/Llm/` — `search_pulse_docs`, `list_pulse_component_classes`
- [x] `Nmsdk-MotionControlLib/Llm/` — `search_motion_control_docs`, `list_motion_control_component_classes`

---

### Этап 3 — Hardening (фаза 3)

- Full test pyramid
- Policy tests
- Session persist
- Hash chain verify
- Rate limits cloud

**Критерий готовности:**
- [x] Cloud session rate limit (`max_cloud_provider_rounds_per_session`)
- [x] L1–L5 unit tests (validator, policy, intent, gateway, orchestrator)
- [x] `Scripts/ci-llm-linux.sh` for CI matrix
- [x] Session persist (TD-008)
- [x] Hash chain verify CLI (TD-010)

**Commit:** `test(rdk-llm): add LLM unit tests, audit log, and CI matrix`

---

### Этап 4 — Embedded llama (обязательно по продукту)

- [x] `RDK_LLM_BUILD_EMBEDDED=ON` + `RDK_LLM_LLAMA_CPP_DIR` / FetchContent b4533
- [x] `UEmbeddedLlamaProvider` + `ULlamaRuntime` (`rdk.llm.embedded`)
- [x] GUI profile `embedded-offline` (Model = path to `.gguf`)
- [x] CI `Scripts/ci-llm-embedded-linux.sh`

**Commit:** `feat(rdk-llm): add embedded llama.cpp provider target`

---

## Post-MVP

### Сценарий C — Workflow operator (реализовано, см. код)

- [x] State machine + `pending_plan` в [Orchestrator.md](Orchestrator.md)
- [x] `ULLMPlanExecutor` + GUI Run plan / Reject
- [x] Rollback: `remove_component` / `set_property` / `disconnect_components` via Rollback or compensate path (TD-018/020)
- [x] Rollback `set_property` when prior value known (TD-020)
- [x] Checkpoints / resume mid-plan (TD-023)

### Сценарий D — Autonomous agent

**Не планировать без:**
- Sandbox tool whitelist ≤ 5
- `max_autonomous_steps ≤ 3`
- Approval every step
- Separate security review

---

## Checklist production-ready (не MVP)

См. [Testing-Strategy.md](Testing-Strategy.md) + [Policy-and-Safety.md](Policy-and-Safety.md):

- [x] RBAC roles in audit (`user_role` on tool_invoke)
- [x] Hash chain verification in CI (via `Test_LLM_AuditChain` in `ci-llm-linux.sh`)
- [x] Schema regression for tools (`ULLMToolSchemaRegression` + unit test)
- [x] No raw prompt in audit (default — `ULLMAuditSanitizer`)
- [x] Provider down hints (cloud → ollama-local; Ollama troubleshooting tip)
- [x] Provider HTTP retry (408/429/5xx, one retry)
- [x] Cloud session rate limit (40 provider rounds)
- [x] Hybrid doc search (TF-IDF + offline semantic boost, TD-017)
- [x] Optional Ollama embeddings re-rank (`NMSDK_LLM_DOC_EMBED_OLLAMA=1`, TD-021)
- [x] `remove_component` write tool
- [x] `connect_components`, `load_project`, `save_project` write tools
- [x] GUI token streaming + Cancel (TD-024)
- [x] HITL confirmation TTL + persist pending (TD-025)
- [x] L7 regression fixtures runner (`Test_LLM_RegressionFixtures`, TD-027)
- [x] Strict `json_schema` plan (cloud OpenAI-compat, TD-028)
- [x] Ollama remote URL/model env + mismatch hints (TD-029)

---

## LLM-first agent (PR0a–PR5) ✅

| PR | Критерий | Статус |
|----|----------|--------|
| PR1 | RU lifecycle, no pre-LLM bypass, tool filter | [x] |
| PR0a | Builtin catalog + `Test_LLM_KnowledgeIndex` | [x] |
| PR0b | `llm-index-pack` → `Bin/LLM/index` | [x] |
| PR2 | Manifest + `no_suitable_tool` recovery | [x] |
| PR3 | `ULLMUserMessages` ru/en | [x] |
| PR4 | Query path hint + `kMinRetrievalScore` | [x] |
| PR5 | `Test_LLM_DocRetrieval`, Agent/Knowledge docs | [x] |

См. [Agent-Interaction.md](Agent-Interaction.md), [Knowledge-Sources.md](Knowledge-Sources.md), [Development-Workflow.md](Development-Workflow.md) § LLM-first.

---

## Post-MVP / backlog (не блокирует MVP)

| Item | Notes |
|------|-------|
| TD-041 | Manual NeuroModeler GUI write-tools walkthrough — [Application-Commands.md](Application-Commands.md) § TD-041 |
| Catalog/index | `llms.txt`, `loadedLibraries()`, incremental index (TD-032/033/039) — done |
| Deferred implementation | [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) — TD-041, TD-031/034/036 |

**Write-tools P1–P3:** завершены — см. [Write-Tools-Backlog.md](Write-Tools-Backlog.md).

---

## Mapping фаз → код

| Фаза | Weeks (оценка) | Targets |
|------|----------------|---------|
| 0 | 1–2 | Docs, CMake, stubs |
| 1 | 2–3 | rdk.llm.core read path, GUI dock |
| 2 | 2–3 | write + HITL |
| 3 | 1–2 | tests, audit hardening |
| 4 | 2–4 | rdk.llm.embedded |
