# Agent documentation (DD-DOC-001)

## RU

Агент отвечает на вопросы по документации **и** открывает её в GUI.

### Поиск / ответ

1. `search_project_docs` (и library `search_*_docs`) → `snippets[]` с `path`, `title`, `excerpt`, **`doc_uri`** (`nmsdk-doc:…`).
2. При ≥2 hits финальный ответ: краткое резюме + markdown-список `[title](doc_uri)`.
3. ClDesc текстом: `describe_class`. Открыть панель класса: `open_class_docs`.

### Открытие в GUI

| Tool | UI |
|------|-----|
| `open_help` / `list_help_topics` | `UHelpWindow` (`Bin/Help/{lang}/*.html`) |
| `open_class_docs` | `UClassDescriptionDisplay` |
| `open_documentation` | `UMarkdownDocWindow` (корни Docs catalog) |

Клики в чате по `nmsdk-doc:` / `nmsdk-help:` / `nmsdk-class:` → тот же `ILLMPresentationSink`.

Path gate: [`ULLMDocOpenPolicy`](../Core/Context/ULLMDocOpenPolicy.h). Индекс: [Knowledge-Sources.md](Knowledge-Sources.md).

---

## EN

The agent both **answers** documentation questions and **opens** Help / ClDesc / markdown Docs in the GUI.

### Search / answer

1. `search_project_docs` (and library `search_*_docs`) → `snippets[]` with `doc_uri` (`nmsdk-doc:…`).
2. When ≥2 hits: short summary + markdown link list `[title](doc_uri)`.
3. ClDesc text: `describe_class`. Open class panel: `open_class_docs`.

### Open in GUI

| Tool | UI |
|------|-----|
| `open_help` / `list_help_topics` | `UHelpWindow` |
| `open_class_docs` | `UClassDescriptionDisplay` |
| `open_documentation` | `UMarkdownDocWindow` |

Chat clicks on `nmsdk-doc|help|class` use the same presentation sink.

See Tools-Contracts § Documentation open / Help.
