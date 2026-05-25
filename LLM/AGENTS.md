# AGENTS — RDK LLM

При работе с подсистемой LLM **сначала** откройте [Docs/README.md](Docs/README.md).

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
| NMSDK paths | [Project-Context-NMSDK.md](Docs/Project-Context-NMSDK.md) |
| CMake | [Build.md](Docs/Build.md) |
| Сборка OFF | [Build.md](Docs/Build.md) §5 |

План-навигатор: `.cursor/plans/rdk_llm_integration_*.plan.md`
