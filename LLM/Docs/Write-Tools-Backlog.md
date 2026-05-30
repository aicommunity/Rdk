# Write-tools — следующая фаза (после MVP)

MVP/post-MVP план закрыт. Дальше: **довести write-tools до production** в NeuroModeler.

## Приоритет P1 (функционал) ✅

| Tool | Статус | Реализация / тесты |
|------|--------|-------------------|
| `add_component` | done | `parent_long_name` → `GetComponentL` + `TakeObject`/`AddComponent` (engine lock); `Test_LLM_WriteToolsEngine` |
| `set_property` | done | `ULLMWriteToolPolicy` low-risk props без HITL |
| `remove_component` | done | Plan compensation TD-018/020; gateway HITL |
| `connect_components` | done | `MModel_CreateLinkByName`; schema fixture |
| `disconnect_components` | done | В mutate filter + registry (plan rollback) |
| `load_*` / `save_*` | done | Path policy, `allow_save`, presentation sink — `Test_LLM_ApplicationFixtures`, policy tests |
| `create_*` / `close_*` | done | TD-030 automated; manual GUI → TD-041 |
| `copy_*` / `rename_*` | done | HITL + path policy tests in `Test_LLM_WriteToolsP1` |
| `validate_configuration` | done | `Test_LLM_E2eLabCommands` |
| Channel calc tools | done | В `kMutateTools`; `Test_LLM_WriteToolsP1` registry |

**Unit gates:** `ctest -R 'Test_LLM_WriteToolsP1|Test_LLM_WriteToolsEngine'` (engine test needs `Bin/Platform/*` + `NeuroModelerConsole`)

## Приоритет P2 (интеграция) ✅

| Item | Статус | Реализация / тесты |
|------|--------|-------------------|
| PulseLib / MotionControlLib | done | Read-only `search_*_docs`, `list_*_classes`; mutations via core `add_component` / `set_property` |
| Library focus from user text | done | `ULLMLibraryScopeHint` in manifest + argument merge (no `add_pulse_*` write tools) |
| HardwareLib write | N/A | Read-only; core `add_component` / `set_property` |
| Entity resolver для write | done | `ULLMWriteArgumentNormalizer` в gateway |
| Plan read-before-write | done | `PLAN_NEEDS_SNAPSHOT` в `checkPlan` |

**Unit gates:** `ctest -R 'Test_LLM_WriteToolsP2'`

## Приоритет P3 (UX / ops) — выполнено ✅

| Item | Статус | Реализация |
|------|--------|------------|
| Новый чат + session при load | done | `ULlmAssistantDockWidget::startNewChat`, `discardSession`, `projectOpened` |
| Ollama model list в Settings | done | editable combo + `Refresh Ollama model list` (`/api/tags`) |
| Embedded live token stream | done | `ULlamaRuntime::completeStream`, TD-045 |

## Как тестировать каждый write-tool

1. `RDK_USE_LLM=ON`, Ollama с chat-моделью + tools (0.3+)
2. Settings: **Allow LLM write tools**; optional **Apply write tools automatically**
3. Фраза mutate → **Apply** (или сразу execute при auto-apply) / plan **Run plan**
4. Audit: `tool_invoke` + `confirmation_accepted` (when HITL used)

## Ссылки

- [Tools-Contracts.md](Tools-Contracts.md)
- [Policy-and-Safety.md](Policy-and-Safety.md)
- [Domain-Layer.md](Domain-Layer.md)
