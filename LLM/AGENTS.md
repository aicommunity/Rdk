# AGENTS — RDK LLM

## RU

При работе с подсистемой LLM **сначала** откройте [Docs/README.md](Docs/README.md).

## Agent v2 (текущая архитектура)

- Единый контракт хода: [Docs/Unified-Turn-Contract.md](Docs/Unified-Turn-Contract.md) (`TurnTerminal`, workflow phases).
- Взаимодействие и HITL: [Docs/Agent-Interaction.md](Docs/Agent-Interaction.md) (`ask_user`, `propose_plan`, task path `HintOnly` по умолчанию).
- Миграция с v1: [Docs/Migration-Agent-V2.md](Docs/Migration-Agent-V2.md).
- Lifecycle: подсказки в ephemeral context; прямой вызов до LLM — `NMSDK_LLM_LIFECYCLE_DIRECT=1`; принудительный `tool_choice` — `NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE=1`.

## Workflow (обязательно)

После **каждой фазы** 0–4: [Development-Workflow.md](Docs/Development-Workflow.md) — checklist, **git commit на английском**, обновить [TECH-DEBT.md](TECH-DEBT.md).

## Быстрые правила

1. Домен = `UApplication` → `UEngine` → `UEnvironment` → `UStorage` → **`UNet`** (фасад `URdkDomainAccess`).
2. Контекст NMSDK (`Bin/`, ClDesc, Docs) — только `ILLMProjectContextProvider`, реализация в `App/NeuroModeler/`.
3. Действия — только `ULLMToolGateway`, не прямой engine.
4. `RDK_USE_LLM=OFF` — не компилировать `Rdk/LLM` и `Rdk/GUI/Qt/Llm`.
5. MVP: read tools (фаза 1) → write + HITL (фаза 2).

## Карта задач → документ

| Задача | Документ |
|--------|----------|
| Новый tool | [Tools-Contracts.md](Docs/Tools-Contracts.md) + [Tool-Gateway-and-Registry.md](Docs/Tool-Gateway-and-Registry.md) |
| Policy / HITL | [Policy-and-Safety.md](Docs/Policy-and-Safety.md) |
| Qt dock | [GUI-Integration.md](Docs/GUI-Integration.md) |
| Orchestrator / turn loop | [Orchestrator.md](Docs/Orchestrator.md), [Unified-Turn-Contract.md](Docs/Unified-Turn-Contract.md) |
| NMSDK paths | [Project-Context-NMSDK.md](Docs/Project-Context-NMSDK.md) |
| CMake | [Build.md](Docs/Build.md) |
| Сборка OFF | [Build.md](Docs/Build.md) §5 |

План-навигатор: `.cursor/plans/rdk_llm_integration_*.plan.md`

---

## EN

When working with the LLM subsystem, **first** open [Docs/README.md](Docs/README.md).

## Agent v2 (current architecture)

- Unified turn contract: [Docs/Unified-Turn-Contract.md](Docs/Unified-Turn-Contract.md) (`TurnTerminal`, workflow phases).
- Interaction and HITL: [Docs/Agent-Interaction.md](Docs/Agent-Interaction.md) (`ask_user`, `propose_plan`, task path `HintOnly` by default).
- Migration from v1: [Docs/Migration-Agent-V2.md](Docs/Migration-Agent-V2.md).
- Lifecycle: hints in ephemeral context; direct call before LLM — `NMSDK_LLM_LIFECYCLE_DIRECT=1`; forced `tool_choice` — `NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE=1`.

## Workflow (required)

After **each phase** 0–4: [Development-Workflow.md](Docs/Development-Workflow.md) — checklist, **git commit in English**, update [TECH-DEBT.md](TECH-DEBT.md).

## Quick rules

1. Domain = `UApplication` → `UEngine` → `UEnvironment` → `UStorage` → **`UNet`** (facade `URdkDomainAccess`).
2. NMSDK context (`Bin/`, ClDesc, Docs) — only via `ILLMProjectContextProvider`, implementation in `App/NeuroModeler/`.
3. Actions — only `ULLMToolGateway`, not direct engine access.
4. `RDK_USE_LLM=OFF` — do not compile `Rdk/LLM` and `Rdk/GUI/Qt/Llm`.
5. MVP: read tools (phase 1) → write + HITL (phase 2).

## Task map → document

| Task | Document |
|--------|----------|
| New tool | [Tools-Contracts.md](Docs/Tools-Contracts.md) + [Tool-Gateway-and-Registry.md](Docs/Tool-Gateway-and-Registry.md) |
| Policy / HITL | [Policy-and-Safety.md](Docs/Policy-and-Safety.md) |
| Qt dock | [GUI-Integration.md](Docs/GUI-Integration.md) |
| Orchestrator / turn loop | [Orchestrator.md](Docs/Orchestrator.md), [Unified-Turn-Contract.md](Docs/Unified-Turn-Contract.md) |
| NMSDK paths | [Project-Context-NMSDK.md](Docs/Project-Context-NMSDK.md) |
| CMake | [Build.md](Docs/Build.md) |
| OFF build | [Build.md](Docs/Build.md) §5 |

Plan navigator: `.cursor/plans/rdk_llm_integration_*.plan.md`
