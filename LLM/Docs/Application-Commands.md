# Application commands (LLM)

## RU

Headless-safe layer over `UApplication` for configuration lifecycle and related operations.

## Architecture

- **`URdkApplicationCommands`** — `UApplication` lifecycle API (CreateProject, OpenProject, …).
- **`ILLMPresentationSink`** — NeuroModeler: `invokeHostSynchronized` runs commands on the **GUI thread**; `apply` refreshes shell. Headless: no sink → commands run on the orchestrator worker thread.

## Phase 1 tools (lifecycle)

| Tool | UApplication | `requires_project_loaded` | Presentation |
|------|--------------|---------------------------|--------------|
| `create_configuration` | `CreateProject` | false | Full shell + context + recent |
| `load_configuration` | `OpenProject` | false | Full shell + context + recent |
| `save_configuration` | `SaveProject` | true | None |
| `save_configuration_as` | `SaveProjectAs` | true | Full shell + context |
| `close_configuration` | `CloseProject` | true | Full shell + context (closed) |

Aliases: `load_project`, `save_project` (deprecated names, same handlers).

## Phase 2 tools

| Tool | Notes |
|------|--------|
| `update_configuration` | `UpdateProject` from JSON |
| `validate_configuration` | `ValidateProject` on disk (not `validate_project` dry-run) |
| `save_project_metadata` | `SaveProjectConfig` |
| `reload_configuration_parameters` | `ReloadParameters` |
| `copy_configuration` | `CopyProject`, optional `switch_to_copy` |
| `rename_configuration` | `RenameProject` (full directory path) |

## Phase 2b (channels)

`start_channel_calculation`, `pause_channel_calculation`, `reset_channel_calculation`, `step_channel_calculation`, `run_n_steps` — `channel_index: -1` = all channels (`run_n_steps` also requires `steps` in `[1, 10000]`).

Channel CRUD: `add_channel`, `delete_channel` (not channel 0), `clone_channel`.

**DD-CALC-001 FastPath (packs):** user phrases «запусти расчет» / `start calculation` (also pause/reset/step + RU/EN synonyms) are handled via Capability Pack `channel_calc` `tryRecorded` → `recordedToolInvoke` (`ULLMChannelCalcCommand`), before TaskPath/ReAct. Reply and Tools block show the outcome; if no configuration is open, the tool error is shown (not an LLM essay). Autonomous whitelist includes these tools and `ask_user`.

### Graph / component tools (domain)

Registered in `RegisterCoreRdkTools.cpp` via `URdkDomainAccess`:

| Tool | Domain / C-API |
|------|----------------|
| `clone_component` | clone under same parent |
| `move_component` | `Model_MoveComponent` |
| `rename_component` | `UContainer::SetName` |
| `reorder_component` | `Model_ChangeComponentPosition` (`step` ±1) |
| `export_component` / `import_component` | `Model_Save/LoadComponentTo/FromFile` + path policy |
| `calculate_component` / `reset_component` / `default_component` | `Env_Calculate` / `Env_Reset` / `Env_Default` |
| `select_component` | `ILLMPresentationSink::navigateToDiagramScope` (not `Env_Select`) |

## Recent configurations and UI panels (phases D/E)

These tools are registered in `RegisterApplicationTools.cpp`:

- `list_recent_configurations` / `open_recent_configuration` use presentation-sink helper data (`ILLMPresentationSink::recentConfigurationPaths`) and `UApplication::GetLastProjectsList()` to provide a unified deduped recent list.
- After `list_recent_configurations`, the orchestrator stores `pending_tool_arguments` for `open_recent_configuration`. Follow-up messages such as `10` or `open` invoke the write tool directly (no spurious LLM `Done.`).
- `list_ui_panels` returns `{id,title,visible}` based on the GUI host (`UGEngineControlWidget`).
- `show_ui_panel` / `open_component_gui_tab` do not call Qt directly from tool handlers; instead they request a GUI action via `LLMPresentationEvent.show_panel`, applied by `ULlmQtPresentationSink` on the GUI thread.

### Watch series and MDI (DD-WATCH-001)

Registered in `RegisterApplicationTools.cpp` via `invokeApplicationTool` → `ILLMPresentationSink::watch*` → `UGEngineControlWidget::llmWatch*` (GUI thread only; no Qt in `URdkApplicationCommands`).

| Tool | Kind | Host |
|------|------|------|
| `add_watch_series` | Write | `llmWatchAddSeries` (`surface=window\|mdi`) |
| `list_watch_series` | Read | `llmWatchListSeries` |
| `remove_watch_series` / `clear_watch_series` | Write | remove/clear |
| `list_watch_mdi` / `create_watch_mdi` / `focus_watch_mdi` / `close_watch_mdi` | Read/Write | MDI `Watches_N` |

`add_watch_series` calls `URdkDomainAccess::validateWatchProperty` (exists + int/double/MDMatrix|MDVector) before host `createSerie` — DomainError path, no Qt.

FastPath: «на график выход X и Y» → `add_watch_series`×N (`ULLMWatchPlotGoal`). Nested («ltzone pneuron», «низкопороговой зоны … PNeuron») → `resolveNestedWatchTarget` → `PNeuron.LTZone` + `Output` (DD-WATCH-002). Spaced `long_name` / `Child.Property` normalized in `normalizeWriteToolArguments`. Autonomous whitelist treats UI/watch tools like `ask_user` (no write-step burn).

**Chat name autocomplete:** `ULlmChatInputCompleter` on assistant input — dictionary from `listNetSnapshot` long_names (all nesting), `listRegisteredClasses` (ClassesList), and property names. Popup opens/refines while typing (token length ≥ 2 or scoped `.`/`:`). **Tab** or **Enter** accepts the highlighted item; **Shift+Tab** / ↑↓ cycle; **Escape** dismisses.

## Policy

- **P01:** write tools need open project except `create_configuration` / `load_configuration` / `validate_configuration` and GUI/UX helpers (`open_recent_configuration`, `show_ui_panel`, `open_component_gui_tab`).
- **P03:** save tools need `session.allow_save`.
- **P04:** paths under Configs / workdir / user config / open project path (`ULLMPathPolicy`).

## Lifecycle argument gate

For natural phrases such as `load config` **without a path**, the orchestrator does **not** call the LLM. It returns `needs_argument_clarification` with a structured prompt (`ULLMLifecycleArgumentGate`). The next user message is merged into pending arguments; when complete, the lifecycle tool runs directly (then Apply in GUI if confirmation is required).

**Create without path:** phrases like `create new config` or `create new project` (RU: `создай новый конфиг`, `создай новый проект`) are handled LLM-first: the model calls `create_configuration` with `parent_directory` and/or `autocreate_subdirectory` (same semantics as File → New with “Autocreate configuration folder?” = Yes). Path resolution is centralized in `UApplication::PrepareNewProjectIniPath` / `GetDefaultConfigsDirectory`, shared with GUI and `URdkApplicationCommands::resolveProjectIniPath`.

## Headless

Call `LLMServices::initialize(app, ctx)` without `setPresentationSink` — commands and audit work; no GUI update.

## Tests

| Target | Purpose |
|--------|---------|
| `Test_LLM_ApplicationFixtures` | JSON fixtures, policy path deny, registry tools |
| `Test_LLM_PresentationSink` | FullShell vs None, audit field attachment |
| `Test_LLM_WatchTools` | Watch/MDI schema registration + sink mock |
| `Test_LLM_OllamaLabIntegration` | HTTP/chat to `http://10.245.1.12:11434` — **skipped** if host down (`GTEST_SKIP`) |
| `Test_LLM_E2eLabCommands` | Orchestrator E2E: user → lab Ollama → tool calls (`validate_configuration`, `load_configuration`, …) — **skipped** if Ollama down |
| `Test_LLM_E2eScenarios` | Parameterized NL E2E: fuzzy RU/EN prompts incl. `создай новый проект` — **skipped** if Ollama down |
| `Test_LLM_LifecycleArgumentGate` | Path extraction, preflight, merge, prompt formatting |
| `Test_LLM_OrchestratorLifecycleArgs` | LLM-first lifecycle: `load config` args, `create new config` / `создай новый проект` mock tool_call |
| `Test_LLM_DocRetrieval` | 16 fixture queries vs builtin index (`llm_retrieval_expectations.json`) |
| `Test_LLM_KnowledgeIndex` | Catalog build; `UApplication` in `scope=all` |

### Manual E2E checklist (TD-041)

Automated gate: `Test_LLM_WriteToolsAudit` (mock LLM → `confirmation_requested`) and
`Test_LLM_OrchestratorLifecycleArgs`. Manual walkthrough in NeuroModeler with Ollama lab
(`NMSDK_LLM_OLLAMA_*`) and real `Configs/`:

1. RU: «создай новый проект» → first tool_call `create_configuration` (not `add_component`).
2. Confirm HITL → configuration folder created and shell refreshes.
3. RU: «загрузи конфигурацию» with path → `load_configuration` or argument clarification.
4. Query: «что такое HardwareLib» → `search_project_docs` with `scope=docs`, cites path.
5. Mutate (project open): «добавь MatrixSource» → `add_component` after Confirm; audit has `tool_invoke_start`.
6. Save: «сохрани конфигурацию» → `save_configuration` or `save_project` after Confirm.
7. **Auto-apply:** enable **Apply write tools automatically**; «добавь нейрон» / create config runs without per-step Apply (no crash in `UImagesWidget`).
8. **Reject then retry:** open last config → create new (HITL) → Reject → enable auto-apply → create again succeeds.

Lab Ollama model: env `NMSDK_LLM_OLLAMA_MODEL` (default `qwen2.5:14b`). Presentation timeout: `NMSDK_LLM_PRESENTATION_TIMEOUT_MS` (default 30000).

Record result in [TECH-DEBT.md](../TECH-DEBT.md) TD-041 when complete.

---

## EN

Headless-safe layer over `UApplication` for configuration lifecycle and related operations.

## Architecture

- **`URdkApplicationCommands`** — `UApplication` lifecycle API (CreateProject, OpenProject, …).
- **`ILLMPresentationSink`** — NeuroModeler: `invokeHostSynchronized` runs commands on the **GUI thread**; `apply` refreshes shell. Headless: no sink → commands run on the orchestrator worker thread.

## Phase 1 tools (lifecycle)

| Tool | UApplication | `requires_project_loaded` | Presentation |
|------|--------------|---------------------------|--------------|
| `create_configuration` | `CreateProject` | false | Full shell + context + recent |
| `load_configuration` | `OpenProject` | false | Full shell + context + recent |
| `save_configuration` | `SaveProject` | true | None |
| `save_configuration_as` | `SaveProjectAs` | true | Full shell + context |
| `close_configuration` | `CloseProject` | true | Full shell + context (closed) |

Aliases: `load_project`, `save_project` (deprecated names, same handlers).

## Phase 2 tools

| Tool | Notes |
|------|--------|
| `update_configuration` | `UpdateProject` from JSON |
| `validate_configuration` | `ValidateProject` on disk (not `validate_project` dry-run) |
| `save_project_metadata` | `SaveProjectConfig` |
| `reload_configuration_parameters` | `ReloadParameters` |
| `copy_configuration` | `CopyProject`, optional `switch_to_copy` |
| `rename_configuration` | `RenameProject` (full directory path) |

## Phase 2b (channels)

`start_channel_calculation`, `pause_channel_calculation`, `reset_channel_calculation`, `step_channel_calculation`, `run_n_steps` — `channel_index: -1` = all channels (`run_n_steps` also requires `steps` in `[1, 10000]`).

Channel CRUD: `add_channel`, `delete_channel` (not channel 0), `clone_channel`.

**DD-CALC-001 FastPath (packs):** user phrases «запусти расчет» / `start calculation` (also pause/reset/step + RU/EN synonyms) are handled via Capability Pack `channel_calc` `tryRecorded` → `recordedToolInvoke` (`ULLMChannelCalcCommand`), before TaskPath/ReAct. Reply and Tools block show the outcome; if no configuration is open, the tool error is shown (not an LLM essay). Autonomous whitelist includes these tools and `ask_user`.

### Graph / component tools (domain)

Registered in `RegisterCoreRdkTools.cpp` via `URdkDomainAccess`:

| Tool | Domain / C-API |
|------|----------------|
| `clone_component` | clone under same parent |
| `move_component` | `Model_MoveComponent` |
| `rename_component` | `UContainer::SetName` |
| `reorder_component` | `Model_ChangeComponentPosition` (`step` ±1) |
| `export_component` / `import_component` | `Model_Save/LoadComponentTo/FromFile` + path policy |
| `calculate_component` / `reset_component` / `default_component` | `Env_Calculate` / `Env_Reset` / `Env_Default` |
| `select_component` | `ILLMPresentationSink::navigateToDiagramScope` (not `Env_Select`) |

## Recent configurations and UI panels (phases D/E)

These tools are registered in `RegisterApplicationTools.cpp`:

- `list_recent_configurations` / `open_recent_configuration` use presentation-sink helper data (`ILLMPresentationSink::recentConfigurationPaths`) and `UApplication::GetLastProjectsList()` to provide a unified deduped recent list.
- After `list_recent_configurations`, the orchestrator stores `pending_tool_arguments` for `open_recent_configuration`. Follow-up messages such as `10` or `open` invoke the write tool directly (no spurious LLM `Done.`).
- `list_ui_panels` returns `{id,title,visible}` based on the GUI host (`UGEngineControlWidget`).
- `show_ui_panel` / `open_component_gui_tab` do not call Qt directly from tool handlers; instead they request a GUI action via `LLMPresentationEvent.show_panel`, applied by `ULlmQtPresentationSink` on the GUI thread.

### Watch series and MDI (DD-WATCH-001)

Same as RU section: `add/list/remove/clear_watch_series`, `list/create/focus/close_watch_mdi` via presentation sink → `UGEngineControlWidget::llmWatch*` (GUI thread).

## Policy

- **P01:** write tools need open project except `create_configuration` / `load_configuration` / `validate_configuration` and GUI/UX helpers (`open_recent_configuration`, `show_ui_panel`, `open_component_gui_tab`).
- **P03:** save tools need `session.allow_save`.
- **P04:** paths under Configs / workdir / user config / open project path (`ULLMPathPolicy`).

## Lifecycle argument gate

For natural phrases such as `load config` **without a path**, the orchestrator does **not** call the LLM. It returns `needs_argument_clarification` with a structured prompt (`ULLMLifecycleArgumentGate`). The next user message is merged into pending arguments; when complete, the lifecycle tool runs directly (then Apply in GUI if confirmation is required).

**Create without path:** phrases like `create new config` or `create new project` (RU: `create new конфиг`, `create new проект`) are handled LLM-first: the model calls `create_configuration` with `parent_directory` and/or `autocreate_subdirectory` (same semantics as File → New with “Autocreate configuration folder?” = Yes). Path resolution is centralized in `UApplication::PrepareNewProjectIniPath` / `GetDefaultConfigsDirectory`, shared with GUI and `URdkApplicationCommands::resolveProjectIniPath`.

## Headless

Call `LLMServices::initialize(app, ctx)` without `setPresentationSink` — commands and audit work; no GUI update.

## Tests

| Target | Purpose |
|--------|---------|
| `Test_LLM_ApplicationFixtures` | JSON fixtures, policy path deny, registry tools |
| `Test_LLM_PresentationSink` | FullShell vs None, audit field attachment |
| `Test_LLM_WatchTools` | Watch/MDI schema registration + sink mock |
| `Test_LLM_OllamaLabIntegration` | HTTP/chat to `http://10.245.1.12:11434` — **skipped** if host down (`GTEST_SKIP`) |
| `Test_LLM_E2eLabCommands` | Orchestrator E2E: user → lab Ollama → tool calls (`validate_configuration`, `load_configuration`, …) — **skipped** if Ollama down |
| `Test_LLM_E2eScenarios` | Parameterized NL E2E: fuzzy RU/EN prompts incl. `create new проект` — **skipped** if Ollama down |
| `Test_LLM_LifecycleArgumentGate` | Path extraction, preflight, merge, prompt formatting |
| `Test_LLM_OrchestratorLifecycleArgs` | LLM-first lifecycle: `load config` args, `create new config` / `create new проект` mock tool_call |
| `Test_LLM_DocRetrieval` | 16 fixture queries vs builtin index (`llm_retrieval_expectations.json`) |
| `Test_LLM_KnowledgeIndex` | Catalog build; `UApplication` in `scope=all` |

### Manual E2E checklist (TD-041)

Automated gate: `Test_LLM_WriteToolsAudit` (mock LLM → `confirmation_requested`) and
`Test_LLM_OrchestratorLifecycleArgs`. Manual walkthrough in NeuroModeler with Ollama lab
(`NMSDK_LLM_OLLAMA_*`) and real `Configs/`:

1. RU: «create new project» → first tool_call `create_configuration` (not `add_component`).
2. Confirm HITL → configuration folder created and shell refreshes.
3. RU: «load configuration» with path → `load_configuration` or argument clarification.
4. Query: «what such as HardwareLib» → `search_project_docs` with `scope=docs`, cites path.
5. Mutate (project open): «add MatrixSource» → `add_component` after Confirm; audit has `tool_invoke_start`.
6. Save: «save configuration» → `save_configuration` or `save_project` after Confirm.
7. **Auto-apply:** enable **Apply write tools automatically**; «add neuron» / create config runs without per-step Apply (no crash in `UImagesWidget`).
8. **Reject then retry:** open last config → create new (HITL) → Reject → enable auto-apply → create again succeeds.

Lab Ollama model: env `NMSDK_LLM_OLLAMA_MODEL` (default `qwen2.5:14b`). Presentation timeout: `NMSDK_LLM_PRESENTATION_TIMEOUT_MS` (default 30000).

Record result in [TECH-DEBT.md](../TECH-DEBT.md) TD-041 when complete.
