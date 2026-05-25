# Conversation State

## 1. Принцип

**Не полагаться на память модели.** Состояние диалога — в `ULLMConversationStore` (per session).

---

## 2. Идентификаторы сессии

| ID | Генерация | Назначение |
|----|-----------|------------|
| `session_id` | UUID / фикс. GUI `gui-session` | Conversation store key |
| `trace_id` | UUID per user message | Audit correlation |
| `confirmation_id` | UUID per pending write | HITL |

`turn_id` — **post-MVP** (не в store).

---

## 3. `ConversationState` (код)

```cpp
struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;
    std::optional<PendingConfirmation> pending;
    std::optional<ULLMExecutionPlan> pending_plan;
    LLMWorkflowPhase workflow_phase = LLMWorkflowPhase::Idle;
    int cloud_provider_rounds = 0;
};

struct PendingConfirmation {
    std::string confirmation_id;
    ToolInvokeRequest request;  // один write-tool
};
```

**Хранение:** `persistToDisk` → `<storage_dir>/<session_id>.json` (в т.ч. `pending_plan` со статусами шагов и `last_result`).

`resolved_entities` / `last_gui_context` — **не** в MVP store (entity resolution stateless per call).

---

## 4. `LLMMessage`

См. `Rdk/LLM/Core/LlmTypes.h` — роли User/Assistant/Tool, `assistant_tool_calls` для Ollama/OpenAI loops.

---

## 5. `ULLMExecutionPlan`

```json
{
  "plan_id": "plan_…",
  "paused": false,
  "checkpoint_after_step_id": 0,
  "requires_user_confirmation": true,
  "steps": [
    {
      "step_id": 1,
      "tool_name": "get_net_snapshot",
      "arguments": {},
      "status": "done",
      "last_result": {}
    }
  ]
}
```

- План парсится из ответа LLM (`parseExecutionPlanFromAssistantText`), не strict json_schema API.
- При ошибке execute: `paused=true`, Resume/Rollback в GUI.
- `prepareExecutionPlanForResume()` сбрасывает `failed`/`skipped` → `pending`.

---

## 6. `PendingConfirmation`

Один `ToolInvokeRequest` на подтверждение. GUI: **Apply** → `confirmPending()`.

TTL auto-expire — **post-MVP** (не реализован).

---

## 7. Очистка контекста

| Событие | Действие |
|---------|----------|
| Load other project | рекомендуется новый `session_id` |
| User «новый чат» | clear messages (post-MVP UI) |
| Reject plan | `rejectPending()` + `pending_plan` reset |
| Rollback plan | `rollbackPlanExecution()` |
