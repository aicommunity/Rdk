# Application commands (LLM)

Headless-safe layer over `UApplication` for configuration lifecycle and related operations.

## Architecture

- **`URdkApplicationCommands`** — all `UApplication` calls from LLM tools (worker thread).
- **`ILLMPresentationSink`** — optional UI refresh after success (`ULlmQtPresentationSink` in NeuroModeler, `ULLMNoopPresentationSink` or `nullptr` headless).

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

`start_channel_calculation`, `pause_channel_calculation`, `reset_channel_calculation`, `step_channel_calculation` — `channel_index: -1` = all channels.

## Policy

- **P01:** write tools need open project except `create_configuration` / `load_configuration` / `validate_configuration`.
- **P03:** save tools need `session.allow_save`.
- **P04:** paths under Configs / workdir / user config / open project path (`ULLMPathPolicy`).

## Lifecycle argument gate

For natural phrases such as `load config` **without a path**, the orchestrator does **not** call the LLM. It returns `needs_argument_clarification` with a structured prompt (`ULLMLifecycleArgumentGate`). The next user message is merged into pending arguments; when complete, the lifecycle tool runs directly (then Apply in GUI if confirmation is required).

**Create without path:** phrases like `create new config` / `создай новый конфиг` / `создай новый проект` are handled LLM-first: the model calls `create_configuration` with `parent_directory` and/or `autocreate_subdirectory` (same semantics as File → New with “Autocreate configuration folder?” = Yes). Path resolution is centralized in `UApplication::PrepareNewProjectIniPath` / `GetDefaultConfigsDirectory`, shared with GUI and `URdkApplicationCommands::resolveProjectIniPath`.

## Headless

Call `LLMServices::initialize(app, ctx)` without `setPresentationSink` — commands and audit work; no GUI update.

## Tests

| Target | Purpose |
|--------|---------|
| `Test_LLM_ApplicationFixtures` | JSON fixtures, policy path deny, registry tools |
| `Test_LLM_PresentationSink` | FullShell vs None, audit field attachment |
| `Test_LLM_OllamaLabIntegration` | HTTP/chat to `http://10.245.1.12:11434` — **skipped** if host down (`GTEST_SKIP`) |
| `Test_LLM_E2eLabCommands` | Orchestrator E2E: user → lab Ollama → tool calls (`validate_configuration`, `load_configuration`, …) — **skipped** if Ollama down |
| `Test_LLM_E2eScenarios` | Parameterized NL E2E: fuzzy RU/EN prompts incl. `создай новый проект` — **skipped** if Ollama down |
| `Test_LLM_LifecycleArgumentGate` | Path extraction, preflight, merge, prompt formatting |
| `Test_LLM_OrchestratorLifecycleArgs` | LLM-first lifecycle: `load config` args, `create new config` / `создай новый проект` mock tool_call |
| `Test_LLM_DocRetrieval` | 16 fixture queries vs builtin index (`llm_retrieval_expectations.json`) |
| `Test_LLM_KnowledgeIndex` | Catalog build; `UApplication` in `scope=all` |

### Manual E2E checklist (TD-030)

Run in NeuroModeler with Ollama lab (`NMSDK_LLM_OLLAMA_*`) and real `Configs/`:

1. RU: «создай новый проект» → first tool_call `create_configuration` (not `add_component`).
2. Confirm HITL → configuration folder created and shell refreshes.
3. RU: «загрузи конфигурацию» with path → `load_configuration` or argument clarification.
4. Query: «что такое HardwareLib» → `search_project_docs` with `scope=docs`, cites path.

Lab Ollama model: env `NMSDK_LLM_OLLAMA_MODEL` (default `qwen2.5:14b`). Presentation timeout: `NMSDK_LLM_PRESENTATION_TIMEOUT_MS` (default 30000).
