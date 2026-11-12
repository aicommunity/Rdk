# RDK LLM — документация проектирования

## RU

## Назначение

Пакет документов описывает подсистему **LLM-ассистента** для конфигурирования схем и проектов RDK/NMSDK. Цель — **исключить неоднозначности** при реализации: агенты и разработчики должны опираться на эти файлы, а не на устные договорённости.

**Код:** `Rdk/LLM/` (ядро), `Rdk/GUI/Qt/Llm/` (UI), `App/NeuroModeler/` (регистрация NMSDK), опционально `Libraries/*/Llm/`.

**Сборка:** `RDK_USE_LLM=OFF` по умолчанию — см. [Build.md](Build.md).

---

## EN

## Glossary

| Term | Meaning |
|--------|----------|
| **Domain** | Layer `UApplication` → `UEngine` → `UEnvironment` → `UStorage`; unit models — component в **`UNet`** |
| **Tool** | Typed action с JSON Schema; the only way to change the system |
| **Tool Gateway** | `ULLMToolGateway` — validation, policy, audit, idempotency before Domain |
| **Project context** | Directory NMSDK: `Bin/`, ClDesc, Docs libraries — via `ILLMProjectContextProvider` |
| **LongName** | Canonical string ID component в runtime (для write) |
| **ClassName** | Имя class RDK (`ArduinoBoard`, `NModel`, …) из `UStorage` / ClDesc |
| **HITL** | Human-in-the-loop — preview + Confirm before write |
| **Plan / Execute** | LLM builds plan; execution only after policy и (для write) confirmation |
| **Capability Pack** | Extensibility unit: match + hints + optional Recorded strategy — [Capability-Packs.md](Capability-Packs.md) |

**Agent spine roadmap:** [Capability-Packs.md](Capability-Packs.md) (DD-PACK-001); tech debt TD-158…TD-165 in [TECH-DEBT.md](../TECH-DEBT.md).

---
