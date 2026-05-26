# Write-tools — следующая фаза (после MVP)

MVP/post-MVP план закрыт. Дальше: **довести write-tools до production** в NeuroModeler.

## Приоритет P1 (функционал)

| Tool | Статус | Задачи |
|------|--------|--------|
| `add_component` | есть | E2E на реальной схеме, parent/long_name edge cases |
| `set_property` | есть | Whitelist «безопасных» свойств без HITL (policy) |
| `remove_component` | есть | Rollback в планах, orphan links |
| `connect_components` | есть | Валидация портов, undo в GUI |
| `load_project` / `load_configuration` | есть | Path policy P04, GUI refresh via presentation sink |
| `save_project` / `save_configuration` | есть | `allow_save` P03, presentation sink |
| `create_configuration` / `close_configuration` | есть | Application command layer; automated E2E (TD-030 done); manual GUI → TD-041 |
| `copy_configuration` / `rename_configuration` | есть | HITL + path policy |
| `validate_configuration` | есть | On-disk `ValidateProject` (≠ `validate_project` dry-run) |
| Channel calc tools | есть | start/pause/reset/step |
| `disconnect_components` | internal | Экспонировать в API LLM или оставить только в plan rollback |

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
