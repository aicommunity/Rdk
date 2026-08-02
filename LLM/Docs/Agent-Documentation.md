# Agent documentation (DD-DOC-001 / DD-DOC-002)

## RU

Агент отвечает на вопросы по документации **и** открывает её в GUI.

### Поиск / ответ

1. `search_project_docs` (и library `search_*_docs`) → `snippets[]` с `path`, `title`, `excerpt`, **`doc_uri`** (`nmsdk-doc:…`).
2. При ≥2 hits финальный ответ: краткое резюме + markdown-список `[title](doc_uri)`.
3. ClDesc текстом: `describe_class`. Открыть панель класса: `open_class_docs`.

### Каталог классов / inventory (DD-DOC-002)

Запросы вида «какие / доступны / список / модели / классы»:

1. Сначала `list_*_component_classes` или `list_registered_classes` — не эссе про один класс.
2. Затем `search_*_docs` по именам/семействам; при «дай ссылки / документацию» — MUST markdown `nmsdk-doc:`.
3. Если модель не вставила ссылки, а в tool_trace есть `doc_uri`, оркестратор **детерминированно** дописывает footer (audit `docs_links_appended`).
4. Autonomous / AoC: `search_*_docs`, `list_*_component_classes`, `open_*` — read whitelist.

Pulse (нейронные модели): hint семейств CSNM / iaF / Izhikevich / cable + полный список из `list_pulse_component_classes`.

### Открытие в GUI

| Tool | UI |
|------|-----|
| `open_help` / `list_help_topics` | `UHelpWindow` (`Bin/Help/{lang}/*.html`) |
| `open_class_docs` | `UClassDescriptionDisplay` |
| `open_documentation` | `UMarkdownDocWindow` (корни Docs catalog) |

Клики в чате по `nmsdk-doc:` / `nmsdk-help:` / `nmsdk-class:` → тот же `ILLMPresentationSink`.

Path gate: [`ULLMDocOpenPolicy`](../Core/Context/ULLMDocOpenPolicy.h). Helpers: [`ULLMDocCatalogHelpers`](../Core/Context/ULLMDocCatalogHelpers.h). Индекс: [Knowledge-Sources.md](Knowledge-Sources.md).

---

## EN

The agent both **answers** documentation questions and **opens** Help / ClDesc / markdown Docs in the GUI.

### Search / answer

1. `search_project_docs` (and library `search_*_docs`) → `snippets[]` with `doc_uri` (`nmsdk-doc:…`).
2. When ≥2 hits: short summary + markdown link list `[title](doc_uri)`.
3. ClDesc text: `describe_class`. Open class panel: `open_class_docs`.

### Catalog inventory (DD-DOC-002)

For “which / available / list / models / classes” questions:

1. Call `list_*_component_classes` / `list_registered_classes` first — not a single-class essay.
2. Then `search_*_docs`; when the user asks for links, answers MUST include `nmsdk-doc:` markdown.
3. If prose lacks schemes but tool snippets have `doc_uri`, the orchestrator appends a deterministic footer (`docs_links_appended`).
4. Autonomous / AoC whitelist covers library `search_*_docs` / `list_*_component_classes` / `open_*`.

### Open in GUI

| Tool | UI |
|------|-----|
| `open_help` / `list_help_topics` | `UHelpWindow` |
| `open_class_docs` | `UClassDescriptionDisplay` |
| `open_documentation` | `UMarkdownDocWindow` |

Chat clicks on `nmsdk-doc|help|class` use the same presentation sink.

See Tools-Contracts § Documentation open / Help.
