# RDK LLM — документация проектирования

## Назначение

Пакет документов описывает подсистему **LLM-ассистента** для конфигурирования схем и проектов RDK/NMSDK. Цель — **исключить неоднозначности** при реализации: агенты и разработчики должны опираться на эти файлы, а не на устные договорённости.

**Код:** `Rdk/LLM/` (ядро), `Rdk/GUI/Qt/Llm/` (UI), `App/NeuroModeler/` (регистрация NMSDK), опционально `Libraries/*/Llm/`.

**Сборка:** `RDK_USE_LLM=OFF` по умолчанию — см. [Build.md](Build.md).

---

## Глоссарий

| Термин | Значение |
|--------|----------|
| **Domain** | Слой `UApplication` → `UEngine` → `UEnvironment` → `UStorage`; единица модели — компонент в **`UNet`** |
| **Tool** | Типизированное действие с JSON Schema; единственный способ изменить систему |
| **Tool Gateway** | `ULLMToolGateway` — валидация, policy, audit, idempotency перед Domain |
| **Project context** | Каталог NMSDK: `Bin/`, ClDesc, Docs библиотек — через `ILLMProjectContextProvider` |
| **LongName** | Канонический строковый ID компонента в runtime (для write) |
| **ClassName** | Имя класса RDK (`ArduinoBoard`, `NModel`, …) из `UStorage` / ClDesc |
| **HITL** | Human-in-the-loop — preview + Confirm перед write |
| **Plan / Execute** | LLM строит план; исполнение только после policy и (для write) подтверждения |

---

## Карта документов (что читать когда)

| Задача агента | Документ |
|---------------|----------|
| Общая картина, слои | [Architecture.md](Architecture.md) |
| С чего начать MVP | [MVP-Roadmap.md](MVP-Roadmap.md) |
| CMake, vcpkg, CI | [Build.md](Build.md) |
| UApplication, UNet, фасад | [Domain-Layer.md](Domain-Layer.md) |
| Bin, ClDesc, Libraries | [Project-Context-NMSDK.md](Project-Context-NMSDK.md) |
| Qt, UGEngineControlWidget | [GUI-Integration.md](GUI-Integration.md) |
| JSON Schema tools | [Tools-Contracts.md](Tools-Contracts.md) |
| Registry, gateway flow | [Tool-Gateway-and-Registry.md](Tool-Gateway-and-Registry.md) |
| Права, лимиты, HITL | [Policy-and-Safety.md](Policy-and-Safety.md) |
| Имена компонентов | [Entity-Resolution.md](Entity-Resolution.md) |
| Сессия, план, confirm | [Conversation-State.md](Conversation-State.md) |
| Agent loop | [Orchestrator.md](Orchestrator.md) |
| Логи, trace, версии | [Observability-and-Audit.md](Observability-and-Audit.md) |
| Ollama, OpenAI, llama.cpp | [Providers.md](Providers.md) |
| Тесты | [Testing-Strategy.md](Testing-Strategy.md) |
| Запреты | [Anti-Patterns.md](Anti-Patterns.md) |
| Патчи CMake (до кода) | [Pending-CMake-Changes.md](Pending-CMake-Changes.md) |
| Коммиты, техдолг | [Development-Workflow.md](Development-Workflow.md) |
| Техдолг (живой файл) | [../TECH-DEBT.md](../TECH-DEBT.md) |

**Навигатор верхнего уровня (краткий):** `.cursor/plans/rdk_llm_integration_*.plan.md` — только дорожная карта; при расхождении **приоритет у `Rdk/LLM/Docs/`**.

---

## Жёсткие инварианты (не обсуждаются в реализации)

1. LLM **не** вызывает `UEngine` / `UNet` / файлы напрямую — только `ULLMToolGateway`.
2. `Rdk/LLM` **не** содержит hardcoded путей `Bin/` NMSDK — только `ILLMProjectContextProvider`.
3. При `RDK_USE_LLM=OFF` нет компиляции `Rdk/LLM` и `Rdk/GUI/Qt/Llm`.
4. Write-tools требуют **canonical LongName** после entity resolution.
5. Сценарий D (автономный агент без лимитов) **запрещён** в MVP.

---

## Версия пакета документов

| Поле | Значение |
|------|----------|
| `DOCS_BUNDLE_ID` | `rdk-llm-docs-1.0.0` |
| Дата | 2026-05-25 |
| Согласовано с | Nmsdk / RDK Core, NeuroModeler |

При изменении контрактов tools — инкремент `TOOL_REGISTRY_VERSION` (см. Tool-Gateway-and-Registry.md) и запись в audit.
