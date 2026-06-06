# Conversation State

## RU

## 1. Принцип

**Не полагаться на память модели.** Состояние диалога — в `ULLMConversationStore` (per session).

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
    int64_t created_at_unix_sec;  // TD-025 TTL (default 600 s)
};
```

**Хранение:** `persistToDisk` → `<storage_dir>/<session_id>.json` (в т.ч. `pending`, `pending_plan` со статусами шагов и `last_result`).

`store_schema_version` 2 fields (see `ULLMConversationStore` JSON):

| Field | Purpose |
|-------|---------|
| `last_gui_context` | Last `LLMGuiContextSnapshot` from GUI envelope |
| `resolved_entities` | Cached component/class resolutions (`kind`, `query_key`, `canonical_value`, `channel_index`) |
| `agent_notes` | Orchestrator scratchpad (markdown, max 4 KB) |
| `session_context_seeded` | Bootstrap system block written once per session |
| `session_summary` | Rule-based compaction artifact (P2) |
| `last_session_context` | Last `LLMSessionContext` snapshot from orchestrator (write/autonomy flags for resume; TD-088) |

Per-turn flags still arrive via `LLMRequestEnvelope.session`; on resume the GUI merges persisted `last_session_context` with live user/channel/project state (`ULlmAssistantDockWidget::buildSession`).

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

- План: `parseExecutionPlanFromAssistantText` (markdown ```json или raw JSON). Cloud OpenAI-compat: optional strict `json_schema` (TD-028).
- При ошибке execute: `paused=true`, Resume/Rollback в GUI.
- `prepareExecutionPlanForResume()` сбрасывает `failed`/`skipped` → `pending`.

---

## 6. `PendingConfirmation`

Один `ToolInvokeRequest` на подтверждение. GUI: **Apply** → `confirmPending()`.

**TTL (TD-025):** `confirmation_ttl_seconds` = 600 (10 min) в `LLMPolicyLimits`.  
Истечение: при новом `handleUserMessage`, при `confirmPending`, в GUI — `QTimer` → `rejectPending`.
Audit: `confirmation_expired`.

---

## 7. Очистка контекста

| Событие | Действие |
|---------|----------|
| Load other project | рекомендуется новый `session_id` |
| User «новый чат» | clear messages (post-MVP UI) |
| Reject plan | `rejectPending()` + `pending_plan` reset |
| Rollback plan | `rollbackPlanExecution()` |

---

## EN

## 1. Principle

**Do not rely on model memory.** Conversation state lives in `ULLMConversationStore` (per session).

## 2. Session identifiers

| ID | Generation | Purpose |
|----|------------|---------|
| `session_id` | UUID / fixed GUI `gui-session` | Conversation store key |
| `trace_id` | UUID per user message | Audit correlation |
| `confirmation_id` | UUID per pending write | HITL |

`turn_id` — **post-MVP** (not in store).

---

## 3. `ConversationState` (code)

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
    ToolInvokeRequest request;  // one write-tool
    int64_t created_at_unix_sec;  // TD-025 TTL (default 600 s)
};
```

**Storage:** `persistToDisk` → `<storage_dir>/<session_id>.json` (including `pending`, `pending_plan` with step statuses and `last_result`).

`store_schema_version` 2 fields (see `ULLMConversationStore` JSON):

| Field | Purpose |
|-------|---------|
| `last_gui_context` | Last `LLMGuiContextSnapshot` from GUI envelope |
| `resolved_entities` | Cached component/class resolutions (`kind`, `query_key`, `canonical_value`, `channel_index`) |
| `agent_notes` | Orchestrator scratchpad (markdown, max 4 KB) |
| `session_context_seeded` | Bootstrap system block written once per session |
| `session_summary` | Rule-based compaction artifact (P2) |
| `last_session_context` | Last `LLMSessionContext` snapshot from orchestrator (write/autonomy flags for resume; TD-088) |

Per-turn flags still arrive via `LLMRequestEnvelope.session`; on resume the GUI merges persisted `last_session_context` with live user/channel/project state (`ULlmAssistantDockWidget::buildSession`).

---

## 4. `LLMMessage`

See `Rdk/LLM/Core/LlmTypes.h` — User/Assistant/Tool roles, `assistant_tool_calls` for Ollama/OpenAI loops.

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

- Plan: `parseExecutionPlanFromAssistantText` (markdown ```json or raw JSON). Cloud OpenAI-compat: optional strict `json_schema` (TD-028).
- On execute error: `paused=true`, Resume/Rollback in GUI.
- `prepareExecutionPlanForResume()` resets `failed`/`skipped` → `pending`.

---

## 6. `PendingConfirmation`

One `ToolInvokeRequest` per confirmation. GUI: **Apply** → `confirmPending()`.

**TTL (TD-025):** `confirmation_ttl_seconds` = 600 (10 min) in `LLMPolicyLimits`.  
Expiration: on new `handleUserMessage`, on `confirmPending`, in GUI — `QTimer` → `rejectPending`.
Audit: `confirmation_expired`.

---

## 7. Context cleanup

| Event | Action |
|-------|--------|
| Load other project | new `session_id` recommended |
| User "new chat" | clear messages (post-MVP UI) |
| Reject plan | `rejectPending()` + `pending_plan` reset |
| Rollback plan | `rollbackPlanExecution()` |
