# Write-tools — следующая фаза (после MVP)

MVP/post-MVP план закрыт. Дальше: **довести write-tools до production** в NeuroModeler.

## Приоритет P1 (функционал)

| Tool | Статус | Задачи |
|------|--------|--------|
| `add_component` | есть | E2E на реальной схеме, parent/long_name edge cases |
| `set_property` | есть | Whitelist «безопасных» свойств без HITL (policy) |
| `remove_component` | есть | Rollback в планах, orphan links |
| `connect_components` | есть | Валидация портов, undo в GUI |
| `load_project` | есть | Path policy, sandbox под `Bin/` |
| `save_project` | есть | `allow_save`, перезапись с confirm |
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
