# Write-tools — следующая фаза (после MVP)

MVP/post-MVP план закрыт. Дальше: **довести write-tools до production** в NeuroModeler.

## Приоритет P1 (функционал) ✅

| Tool | Статус | Реализация / тесты |
|------|--------|-------------------|
| `add_component` | done | `parent_long_name` → `MModel_AddComponent`; `Test_LLM_WriteToolsP1` |
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

## Приоритет P2 (интеграция)

- PulseLib / MotionControlLib **write** tools (сейчас read-only Llm/)
- HardwareLib: write только если есть domain API
- Entity resolver для write (canonical long_name в аргументах)
- Plan steps: write после read snapshot в одном плане

## Приоритет P3 (UX / ops)

- «Новый чат» + смена `session_id` при load project
- Embedded live token stream (TD отдельно)
- Ollama: подсказка модели в Settings (список из `/api/tags`) — частично есть

## Как тестировать каждый write-tool

1. `RDK_USE_LLM=ON`, Ollama с chat-моделью + tools (0.3+)
2. Settings: **Allow LLM write tools**, открытый проект
3. Фраза mutate → **Apply** / plan **Run plan**
4. Audit: `tool_invoke` + `confirmation_accepted`

## Ссылки

- [Tools-Contracts.md](Tools-Contracts.md)
- [Policy-and-Safety.md](Policy-and-Safety.md)
- [Domain-Layer.md](Domain-Layer.md)
