# Conversation State

## 1. Принцип

**Не полагаться на память модели.** Состояние диалога — в `ULLMConversationStore` (per session / per project).

Практика: session store с entity IDs, pending confirmations, last plan ([agent production patterns](https://dev.to/murali8k/building-micro-agents-as-production-grade-microservices-f4j)).

---

## 2. Идентификаторы сессии

| ID | Генерация | Назначение |
|----|-----------|------------|
| `session_id` | UUID при открытии dock | Conversation store key |
| `trace_id` | UUID per user message | Audit correlation |
| `turn_id` | monotonic per session | Agent loop round |
| `confirmation_id` | UUID per pending write | HITL |

---

## 3. `ULLMConversationStore`

```cpp
struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;      // роли: system, user, assistant, tool
    std::map<std::string, std::string> resolved_entities; // alias -> long_name
    std::optional<ULLMExecutionPlan> pending_plan;
    std::optional<PendingConfirmation> pending_confirmation;
    LLMGuiContext last_gui_context;
    int active_channel_index = 0;
};

class ULLMConversationStore {
public:
    ConversationState& getOrCreate(const std::string& session_id);
    void appendMessage(const std::string& session_id, LLMMessage msg);
    void setPendingConfirmation(...);
    void clearPending(const std::string& session_id);
    void persistToDisk(const std::string& session_id);  // опционально, фаза 3
};
```

**Хранение MVP:** in-memory; при закрытии проекта — `clear session` или archive в `UserConfig/LLM/sessions/<uuid>.json`.

---

## 4. `LLMMessage`

```cpp
struct LLMMessage {
    enum class Role { System, User, Assistant, Tool };
    Role role;
    std::string content;                   // text
    std::optional<std::string> tool_call_id;
    std::optional<std::string> tool_name;
    std::optional<nlohmann::json> tool_arguments;
    std::optional<nlohmann::json> tool_result;
};
```

**OpenAI/Anthropic mapping:** документировать в [Providers.md](Providers.md) — tool messages interleaved correctly (каждый `tool_use` → `tool_result`).

---

## 5. `ULLMExecutionPlan` (structured)

```json
{
  "plan_id": "uuid",
  "intent": "mutate",
  "steps": [
    {
      "step_id": 1,
      "tool_name": "find_component",
      "arguments": { "query": "Source" },
      "status": "pending"
    },
    {
      "step_id": 2,
      "tool_name": "set_property",
      "arguments": { "long_name": "{{step1.canonical}}", "property_name": "FileName", "value": "data.csv" },
      "status": "pending",
      "depends_on": [1]
    }
  ],
  "requires_user_confirmation": true
}
```

Plan строится orchestrator'ом **до** execute (фаза 2); фаза 1 — только ad-hoc tool loop без persisted plan.

---

## 6. `PendingConfirmation`

```cpp
struct PendingConfirmation {
    std::string confirmation_id;
    std::chrono::system_clock::time_point created_at;
    std::vector<ToolInvokeRequest> queued_invokes;
    std::string summary_text;  // для preview UI
};
```

TTL: 10 minutes → auto `reject` + audit `confirmation_expired`.

---

## 7. Очистка контекста

| Событие | Действие |
|---------|----------|
| Load other project | new session_id recommended |
| User «новый чат» | clear messages, keep provider settings |
| Undo in engine (post-MVP) | invalidate pending plan |

---

## 8. Лимит истории messages

- Хранить последние **40** сообщений в hot store
- Старшие — summarize в system-side «session_summary» (опционально, фаза 3) одним LLM call
