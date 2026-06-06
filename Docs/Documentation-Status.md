# Статус документации Rdk Core

## RU

Краткий статус документации ядра Rdk (`Rdk/Docs/`) и чеклист синхронизации с кодом.

**Аудит:** 2026-06-06 — [Documentation-Inventory.json](../../Docs/Audit/Documentation-Inventory.json), [Link-Health-Report.md](../../Docs/Audit/Link-Health-Report.md), [Bilingual-Parity-Report.md](../../Docs/Audit/Bilingual-Parity-Report.md)

> **Область:** только `Rdk/Docs/`. Документация LLM-агента (`Rdk/LLM/Docs/`) — отдельная зона; GUI markdown (`Rdk/GUI/Qt/static/markdown/`) — вне этого статуса.

---

## Статистика

| Метрика | Значение |
|---------|----------|
| Markdown-файлов в `Rdk/Docs/` | **45** |
| Файлов с секцией `## EN

` | **44** (98%) |
| Файлов RU-only | **1** (уточнить при следующем аудите) |
| Битых исходящих ссылок (зона Rdk/Docs) | **~20** — see Link-Health-Report |
| Битых входящих ссылок из `Docs/` | **~50+** — неверные относительные пути к `Rdk/Docs/` |

**Прогресс:** обзорная и детальная documentation ядра **создана**; основные пробелы — ссылочная целостность, синхронизация API-справочников с заголовками `Rdk/Core/`, отсутствие единого templateа обновления при изменении кода.

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

### Detailed documentation модулей
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
- ✅ **Error-Handling.md** — обработка errors (RU/EN)
- ✅ **Project-Management.md** — управление projectми (RU/EN)

### Примеры (`Examples/`)
- ✅ **README.md** — индекс примеров (RU/EN)
- ✅ **Component-Creation-Example.md** (RU/EN)
- ✅ **Serialization-Example.md** (RU/EN)
- ✅ **RPC-Integration-Example.md** (RU/EN)
- ✅ **Threading-Example.md** (RU/EN)

### Диаграммы (`Diagrams/`)
- ✅ **README.md** — description diagrams (RU/EN)
- ✅ **Component-Lifecycle.md** (RU/EN)
- ✅ **Property-System.md** (RU/EN)
- ✅ **RPC-Flow.md** (RU/EN)

---

## EN

Brief status of Rdk core documentation (`Rdk/Docs/`) and a code-sync checklist.

**Audit:** 2026-06-06 — [Documentation-Inventory.json](../../Docs/Audit/Documentation-Inventory.json), [Link-Health-Report.md](../../Docs/Audit/Link-Health-Report.md), [Bilingual-Parity-Report.md](../../Docs/Audit/Bilingual-Parity-Report.md)

> **Scope:** `Rdk/Docs/` only. LLM agent docs (`Rdk/LLM/Docs/`) are a separate zone; GUI markdown (`Rdk/GUI/Qt/static/markdown/`) is out of scope for this status.

---

## Statistics

| Metric | Value |
|--------|-------|
| Markdown files in `Rdk/Docs/` | **45** |
| Files with `## EN` section | **44** (98%) |
| RU-only files | **1** (confirm on next audit) |
| Broken outgoing links (Rdk/Docs zone) | **~20** — see Link-Health-Report |
| Broken incoming links from `Docs/` | **~50+** — incorrect relative paths to `Rdk/Docs/` |

**Progress:** overview and detailed core documentation **created**; main gaps — link integrity, API reference sync with `Rdk/Core/` headers, no single update template when code changes.

---

## Completed

### Navigation and overview
- ✅ **README.md** — catalog index, quick links (RU/EN)
- ✅ **Getting-Started.md** — role-based quick start (RU/EN)
- ✅ **FAQ.md** — frequently asked questions (RU/EN)
- ✅ **Architecture.md** — subsystem overview (RU/EN)
- ✅ **Architecture-Diagrams.md** — summary diagrams (RU/EN)

### Subsystem architecture (`Architecture/`)
- ✅ **Overview.md** — Rdk Core overview (RU/EN)
- ✅ **Application-Architecture.md** — RPC, projects, server (RU/EN)
- ✅ **Engine-Architecture.md** — components, properties, controllers (RU/EN)
- ✅ **Graphics-Architecture.md** — graphics, rendering (RU/EN)
- ✅ **Serialize-Architecture.md** — XML/Binary serialization (RU/EN)
- ✅ **System-Platform-Abstraction.md** — mutexes, library loading (RU/EN)

### Detailed module documentation
- ✅ **Engine-Detailed.md** — Core/Engine (RU/EN)
- ✅ **Application-Detailed.md** — Core/Application (RU/EN)
- ✅ **Graphics-Detailed.md** — Core/Graphics (RU/EN)
- ✅ **Serialize-Detailed.md** — Core/Serialize (RU/EN)
- ✅ **System-Detailed.md** — Core/System (RU/EN)
- ✅ **Math-Detailed.md** — Core/Math (RU/EN)
- ✅ **Utilities-Detailed.md** — Core/Utilities (RU/EN)

### References
- ✅ **API-Reference.md** — public classes and methods (RU/EN)
- ✅ **Math-Libraries-Reference.md** — matrices, vectors, Kalman (RU/EN)
- ✅ **Utilities-Reference.md** — exceptions, files, timestamps (RU/EN)
- ✅ **Controllers-System.md** — controller system (RU/EN)
- ✅ **Console-Application.md** — console application (RU/EN)
- ✅ **Logging-System.md** — logging (RU/EN)
- ✅ **Configuration-Management.md** — UProject, deploy (RU/EN)
- ✅ **Tests.md** — test structure (RU/EN)

### Guides (`Guides/`)
- ✅ **Component-System.md** — component system (RU/EN)
- ✅ **Creating-Components.md** — creating components (RU/EN)
- ✅ **Creating-Properties.md** — creating properties (RU/EN)
- ✅ **Creating-Controllers.md** — creating controllers (RU/EN)
- ✅ **Component-Development.md** — component development (RU/EN)
- ✅ **Serialization-Guide.md** — serialization (RU/EN)
- ✅ **RPC-Integration.md** — RPC integration (RU/EN)
- ✅ **Threading-Guide.md** — multithreading (RU/EN)
- ✅ **Error-Handling.md** — error handling (RU/EN)
- ✅ **Project-Management.md** — project management (RU/EN)

### Examples (`Examples/`)
- ✅ **README.md** — examples index (RU/EN)
- ✅ **Component-Creation-Example.md** (RU/EN)
- ✅ **Serialization-Example.md** (RU/EN)
- ✅ **RPC-Integration-Example.md** (RU/EN)
- ✅ **Threading-Example.md** (RU/EN)

### Diagrams (`Diagrams/`)
- ✅ **README.md** — diagram descriptions (RU/EN)
- ✅ **Component-Lifecycle.md** (RU/EN)
- ✅ **Property-System.md** (RU/EN)
- ✅ **RPC-Flow.md** (RU/EN)

---
