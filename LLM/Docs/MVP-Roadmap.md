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
| GUI chat + stream | Autonomous loop |
| Ollama + OpenAI-compat | Embedded llama |
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
| Write tools + HITL | Scenario C workflows |
| Plan preview | Scenario D autonomous |
| `add_component`, `set_property`, … | Library-specific tools (optional 2b) |
| Idempotency keys | Parallel write |

**Критерий готовности:**
- [x] Add component на схему после Confirm
- [x] Deny write без open project
- [ ] Ambiguous name → clarification UI

**Commit:** `feat(rdk-llm): write tools with policy, HITL, and NMSDK context`

---

### Этап 2b — Library extensions (опционально параллельно)

- `Libraries/Rdk-HardwareLib/Llm/` — 1–2 read tools + doc paths
- Pulse/MotionControl — по приоритету продукта

---

### Этап 3 — Hardening (фаза 3)

- Full test pyramid
- Policy tests
- Session persist
- Hash chain verify
- Rate limits cloud

**Критерий готовности:**
- [x] L1–L5 unit tests (validator, policy, intent, gateway, orchestrator)
- [x] `Scripts/ci-llm-linux.sh` for CI matrix
- [ ] Session persist (TD-008)
- [ ] Hash chain verify CLI (TD-010)

**Commit:** `test(rdk-llm): add LLM unit tests, audit log, and CI matrix`

---

### Этап 4 — Embedded llama (обязательно по продукту)

- `RDK_LLM_BUILD_EMBEDDED=ON`
- `UEmbeddedLlamaProvider`
- GUI profile In-process GGUF
- CI job `llm-embedded`

**Commit:** `feat(rdk-llm): add embedded llama.cpp provider target`

---

## Post-MVP (только design в Docs)

### Сценарий C — Workflow operator

- State machine в [Orchestrator.md](Orchestrator.md)
- Saga/compensation для failed multi-step
- Checkpoints

### Сценарий D — Autonomous agent

**Не планировать без:**
- Sandbox tool whitelist ≤ 5
- `max_autonomous_steps ≤ 3`
- Approval every step
- Separate security review

---

## Checklist production-ready (не MVP)

См. [Testing-Strategy.md](Testing-Strategy.md) + [Policy-and-Safety.md](Policy-and-Safety.md):

- [ ] RBAC per user
- [ ] Hash chain verification in CI
- [ ] Schema regression for tools ([schema-gateway](https://github.com/sravan27/schema-gateway) pattern)
- [ ] No raw prompt in audit (default)
- [ ] Disaster: provider down → wizard fallback

---

## Mapping фаз → код

| Фаза | Weeks (оценка) | Targets |
|------|----------------|---------|
| 0 | 1–2 | Docs, CMake, stubs |
| 1 | 2–3 | rdk.llm.core read path, GUI dock |
| 2 | 2–3 | write + HITL |
| 3 | 1–2 | tests, audit hardening |
| 4 | 2–4 | rdk.llm.embedded |
