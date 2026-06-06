# Статус документации Rdk Core

Краткий статус документации ядра Rdk (`Rdk/Docs/`) и чеклист синхронизации с кодом.

**Аудит:** 2026-06-06 — [Documentation-Inventory.json](../../Docs/Audit/Documentation-Inventory.json), [Link-Health-Report.md](../../Docs/Audit/Link-Health-Report.md), [Bilingual-Parity-Report.md](../../Docs/Audit/Bilingual-Parity-Report.md)

> **Область:** только `Rdk/Docs/`. Документация LLM-агента (`Rdk/LLM/Docs/`) — отдельная зона; GUI markdown (`Rdk/GUI/Qt/static/markdown/`) — вне этого статуса.

---

## Статистика

| Метрика | Значение |
|---------|----------|
| Markdown-файлов в `Rdk/Docs/` | **45** |
| Файлов с секцией `## EN` | **44** (98%) |
| Файлов RU-only | **1** (уточнить при следующем аудите) |
| Битых исходящих ссылок (зона Rdk/Docs) | **~20** — см. Link-Health-Report |
| Битых входящих ссылок из `Docs/` | **~50+** — неверные относительные пути к `Rdk/Docs/` |

**Прогресс:** обзорная и детальная документация ядра **создана**; основные пробелы — ссылочная целостность, синхронизация API-справочников с заголовками `Rdk/Core/`, отсутствие единого шаблона обновления при изменении кода.

---

## Выполнено

### Навигация и обзор
- ✅ **README.md** — индекс каталога, быстрые ссылки (RU/EN)
- ✅ **Getting-Started.md** — быстрый старт по ролям (RU/EN)
- ✅ **FAQ.md** — часто задаваемые вопросы (RU/EN)
- ✅ **Architecture.md** — обзор подсистем (RU/EN)
- ✅ **Architecture-Diagrams.md** — сводные диаграммы (RU/EN)

### Архитектура подсистем (`Architecture/`)
- ✅ **Overview.md** — обзор Rdk Core (RU/EN)
- ✅ **Application-Architecture.md** — RPC, проекты, сервер (RU/EN)
- ✅ **Engine-Architecture.md** — компоненты, свойства, контроллеры (RU/EN)
- ✅ **Graphics-Architecture.md** — графика, отрисовка (RU/EN)
- ✅ **Serialize-Architecture.md** — XML/Binary сериализация (RU/EN)
- ✅ **System-Platform-Abstraction.md** — мьютексы, загрузка библиотек (RU/EN)

### Детальная документация модулей
- ✅ **Engine-Detailed.md** — Core/Engine (RU/EN)
- ✅ **Application-Detailed.md** — Core/Application (RU/EN)
- ✅ **Graphics-Detailed.md** — Core/Graphics (RU/EN)
- ✅ **Serialize-Detailed.md** — Core/Serialize (RU/EN)
- ✅ **System-Detailed.md** — Core/System (RU/EN)
- ✅ **Math-Detailed.md** — Core/Math (RU/EN)
- ✅ **Utilities-Detailed.md** — Core/Utilities (RU/EN)

### Справочники
- ✅ **API-Reference.md** — публичные классы и методы (RU/EN)
- ✅ **Math-Libraries-Reference.md** — матрицы, векторы, Калман (RU/EN)
- ✅ **Utilities-Reference.md** — исключения, файлы, timestamps (RU/EN)
- ✅ **Controllers-System.md** — система контроллеров (RU/EN)
- ✅ **Console-Application.md** — консольное приложение (RU/EN)
- ✅ **Logging-System.md** — логирование (RU/EN)
- ✅ **Configuration-Management.md** — UProject, деплой (RU/EN)
- ✅ **Tests.md** — структура тестов (RU/EN)

### Руководства (`Guides/`)
- ✅ **Component-System.md** — система компонентов (RU/EN)
- ✅ **Creating-Components.md** — создание компонентов (RU/EN)
- ✅ **Creating-Properties.md** — создание свойств (RU/EN)
- ✅ **Creating-Controllers.md** — создание контроллеров (RU/EN)
- ✅ **Component-Development.md** — разработка компонентов (RU/EN)
- ✅ **Serialization-Guide.md** — сериализация (RU/EN)
- ✅ **RPC-Integration.md** — интеграция RPC (RU/EN)
- ✅ **Threading-Guide.md** — многопоточность (RU/EN)
- ✅ **Error-Handling.md** — обработка ошибок (RU/EN)
- ✅ **Project-Management.md** — управление проектами (RU/EN)

### Примеры (`Examples/`)
- ✅ **README.md** — индекс примеров (RU/EN)
- ✅ **Component-Creation-Example.md** (RU/EN)
- ✅ **Serialization-Example.md** (RU/EN)
- ✅ **RPC-Integration-Example.md** (RU/EN)
- ✅ **Threading-Example.md** (RU/EN)

### Диаграммы (`Diagrams/`)
- ✅ **README.md** — описание диаграмм (RU/EN)
- ✅ **Component-Lifecycle.md** (RU/EN)
- ✅ **Property-System.md** (RU/EN)
- ✅ **RPC-Flow.md** (RU/EN)

---

## Планируется / рекомендации

### Приоритет P0 — ссылочная целостность
- [ ] Исправить исходящие ссылки из `Rdk/Docs/` на несуществующие цели:
  - `Architecture/Overview.md` → `Docs/Submodules/Rdk-Index.md`, `Navigation-Map.md`
  - `Architecture/Graphics-Architecture.md` → `Docs/GUI/Overview.md`, `Style-System.md`
  - `Guides/Component-Development.md` → `Docs/Components-And-Configuration/Component-System.md`, `Docs/Rdk-Core/Engine-Architecture.md`
  - `Guides/Project-Management.md` → `Bin/Docs/Configs-Structure.md` (путь), `Rdk-Core/Application-Architecture.md`
  - `Diagrams/README.md` → `../../Architecture/Overview.md` (неверный относительный путь)
- [ ] Исправить входящие ссылки из корневого `Docs/` — использовать `../Rdk/Docs/...` вместо `Rdk/Docs/...`
- [ ] Устранить ложные «битые ссылки» из фрагментов кода в API/Math docs (`int index`, `const std::string& params`)

### Приоритет P1 — синхронизация с кодом
- [ ] Сверить **API-Reference.md** с публичными заголовками `Rdk/Core/Engine/`, `Application/`, `Serialize/`
- [ ] Сверить **Math-Libraries-Reference.md** / **Math-Detailed.md** с `Rdk/Core/Math/`
- [ ] Сверить **Controllers-System.md** с актуальной иерархией контроллеров
- [ ] Обновить **Tests.md** при добавлении CMake targets (см. [Code-Inventory.json](../../Docs/Audit/Code-Inventory.json) — 39 test targets)
- [ ] Согласовать **Logging-System.md** с sinks из glog-обёртки

### Приоритет P2 — навигация и шаблоны
- [ ] Добавить перекрёстные ссылки на `Libraries/*/Docs/Documentation-Status.md`
- [ ] Создать **CONTRIBUTING-docs.md** для Rdk Core (чеклист обновления при изменении API)
- [ ] Унифицировать дублирование между `Architecture/*.md` и `*-Detailed.md`

---

## Чеклист по разделам

### Обзор и архитектура (11 файлов)
| Документ | RU | EN | Актуальность кода | Ссылки |
|----------|----|----|-------------------|--------|
| README.md | ✅ | ✅ | — | ⚠ входящие из Docs/ |
| Getting-Started.md | ✅ | ✅ | — | ⚠ |
| FAQ.md | ✅ | ✅ | — | ⚠ |
| Architecture.md | ✅ | ✅ | ⚠ периодически | ✅ |
| Architecture-Diagrams.md | ✅ | ✅ | ⚠ | ✅ |
| Architecture/Overview.md | ✅ | ✅ | ⚠ | ❌ Submodules |
| Architecture/Application-Architecture.md | ✅ | ✅ | ⚠ | ⚠ |
| Architecture/Engine-Architecture.md | ✅ | ✅ | ⚠ | ⚠ |
| Architecture/Graphics-Architecture.md | ✅ | ✅ | ⚠ | ❌ GUI links |
| Architecture/Serialize-Architecture.md | ✅ | ✅ | ⚠ | ✅ |
| Architecture/System-Platform-Abstraction.md | ✅ | ✅ | ⚠ | ❌ Cross-Platform |

### Детальные модули (7 файлов)
| Документ | RU | EN | Синхронизация с Core/ |
|----------|----|----|------------------------|
| Engine-Detailed.md | ✅ | ✅ | ⚠ требует ревью |
| Application-Detailed.md | ✅ | ✅ | ⚠ |
| Graphics-Detailed.md | ✅ | ✅ | ⚠ |
| Serialize-Detailed.md | ✅ | ✅ | ⚠ |
| System-Detailed.md | ✅ | ✅ | ⚠ |
| Math-Detailed.md | ✅ | ✅ | ⚠ псевдо-ссылки |
| Utilities-Detailed.md | ✅ | ✅ | ⚠ |

### Справочники (8 файлов)
| Документ | RU | EN | Примечание |
|----------|----|----|------------|
| API-Reference.md | ✅ | ✅ | ⚠ полнота vs заголовки |
| Math-Libraries-Reference.md | ✅ | ✅ | ⚠ |
| Utilities-Reference.md | ✅ | ✅ | ⚠ |
| Controllers-System.md | ✅ | ✅ | ⚠ |
| Console-Application.md | ✅ | ✅ | ✅ |
| Logging-System.md | ✅ | ✅ | ⚠ |
| Configuration-Management.md | ✅ | ✅ | ⚠ |
| Tests.md | ✅ | ✅ | ⚠ 39 CMake targets |

### Руководства (10 файлов) — все ✅ RU/EN
### Примеры (5 файлов) — все ✅ RU/EN; RPC-Example — ложные code-links
### Диаграммы (4 файла) — все ✅ RU/EN; README — битая ссылка на Overview

---

## Чеклист синхронизации с кодом

При изменении API в `Rdk/Core/` рекомендуется обновить:

1. **API-Reference.md** — новые/удалённые публичные классы, методы, типы
2. Соответствующий **\*-Detailed.md** и **Architecture/\*.md** — при изменении поведения подсистемы
3. **Guides/Creating-Components.md**, **Creating-Properties.md**, **Creating-Controllers.md** — при изменении контрактов разработки
4. **Examples/\*.md** — при изменении типовых паттернов использования
5. **Diagrams/\*.md** — при изменении жизненного цикла, property flow, RPC
6. **Tests.md** — при добавлении/переименовании тестовых targets в CMake
7. **Configuration-Management.md** — при изменении UProject/UApplication
8. Корневой **Docs/** — обновить ссылки на `../Rdk/Docs/...`
9. Перезапустить аудит: `Scripts/doc-audit/run-all.sh`

### Зоны кода → документы

| Зона кода | Документы |
|-----------|-----------|
| `Rdk/Core/Engine/` | Engine-Detailed, Architecture/Engine-Architecture, Guides/Component-* |
| `Rdk/Core/Application/` | Application-Detailed, Configuration-Management, RPC-Integration |
| `Rdk/Core/Serialize/` | Serialize-Detailed, Serialization-Guide, Examples/Serialization-* |
| `Rdk/Core/Graphics/` | Graphics-Detailed, Architecture/Graphics-Architecture |
| `Rdk/Core/System/` | System-Detailed, Architecture/System-Platform-Abstraction |
| `Rdk/Core/Math/` | Math-Detailed, Math-Libraries-Reference |
| `Rdk/Core/Utilities/` | Utilities-Detailed, Utilities-Reference |
| `Rdk/GUI/` | Architecture/Graphics-Architecture (перекрёстные ссылки на Docs/GUI/) |

---

## EN

### Documentation status summary

`Rdk/Docs/` contains **45** markdown files; **44** include an `## EN` section (~98% bilingual coverage). Core subsystem docs, guides, examples, and diagrams are in place. Main gaps: **broken cross-links** (to/from root `Docs/`, GUI docs, Submodules index) and **API reference drift** vs `Rdk/Core/` headers.

### Priority actions

1. Fix broken relative links (outbound from Rdk/Docs and inbound from Docs/)
2. Reconcile API-Reference and *-Detailed docs with current Core headers
3. Update Tests.md when CMake test targets change (39 targets in Code-Inventory)

### Sync checklist (code changes)

When changing `Rdk/Core/`: update API-Reference, relevant *-Detailed and Architecture docs, guides, examples, diagrams, Tests.md; fix root Docs/ cross-links; re-run `Scripts/doc-audit/run-all.sh`.

---

*Источники: [Documentation-Inventory.json](../../Docs/Audit/Documentation-Inventory.json), [Link-Health-Report.md](../../Docs/Audit/Link-Health-Report.md), [Code-Inventory.json](../../Docs/Audit/Code-Inventory.json); шаблон — [Nmsdk-PulseLib/Docs/Documentation-Status.md](../../Libraries/Nmsdk-PulseLib/Docs/Documentation-Status.md).*
