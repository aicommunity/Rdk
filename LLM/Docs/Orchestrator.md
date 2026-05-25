# LLM Orchestrator

## 1. `ULLMAgentOrchestrator`

Центральный класс. Зависимости (inject):

- `ILLMProvider&`
- `ULLMToolGateway&`
- `ULLMToolRegistry&`
- `ULLMConversationStore&`
- `ULLMPolicyEngine&`
- `URdkContextRetriever&`
- `URdkEntityResolver&`
- `ULLMAuditLog&`

```cpp
class ULLMAgentOrchestrator {
public:
    void handleUserMessage(const LLMRequestEnvelope& req,
                           std::function<void(const LLMResponseChunk&)> on_chunk,
                           std::function<void(const LLMFinalResponse&)> on_done);

    void confirmPendingExecution(const std::string& session_id,
                                 const std::string& confirmation_id);
    void rejectPendingExecution(const std::string& session_id,
                                const std::string& confirmation_id);
    void cancel(const std::string& session_id);

    bool isRunning(const std::string& session_id) const;
};
```

---

## 2. `LLMRequestEnvelope`

```cpp
struct LLMRequestEnvelope {
    std::string session_id;
    std::string trace_id;
    std::string user_text;
    LLMGuiContext gui_context;
    LLMProviderProfile provider_profile;
    LLMIntentKind intent_hint;  // Auto | Query | Mutate | Explain
};
```

---

## 3. Intent layer (`ULLMIntentParser`)

**Фаза 1:** rule-based + keywords (быстро, детерминировано):

| Паттерн | Intent |
|---------|--------|
| добавь, создай, удали, сохрани, загрузи | Mutate |
| что, покажи, список, опиши, какие | Query |
| почему, объясни | Explain |

**Фаза 3 (опционально):** small LLM call structured JSON `{ "intent": "query" }`.

`Auto` → rule-based; при сомнении → `Query` (безопаснее).

---

## 4. Agent loop (нормативный псевдокод)

```
function handleUserMessage(envelope):
  trace_id = envelope.trace_id
  state = store.get(envelope.session_id)
  append user message
  ctx = retriever.build(envelope.gui_context, state)
  policy.checkRetrievedContext(ctx)
  tools = registry.listForLlmApi(filter(intent))
  round = 0
  while round < MAX_ROUNDS:
    if cancelled: return
    response = provider.chat(state.messages, tools, stream)
    if response.has_tool_calls:
      for each call in response.tool_calls:
        result = gateway.invoke(call with trace_id)
        if result.pending_confirmation:
          state.pending = result
          UI.showPreview()
          return  // wait user
        append tool_result to messages
      round++
      continue
    else:
      append assistant text
      on_done(response)
      return
  on_done(limit_reached_message)
```

`MAX_ROUNDS` default: **8** (policy).

---

## 5. Plan / Execute (фаза 2)

**Plan phase:**
1. LLM с tools disabled, structured output schema `ExecutionPlan`
2. Policy `checkPlan(plan)`
3. Store `pending_plan`, show preview

**Execute phase** (after confirm):
1. For each step in order (respect `depends_on`):
2. `gateway.invoke`
3. Audit `plan_checkpoint` after each successful step
4. On hard error: save `pending_plan` with step statuses + `last_result` (no auto-rollback)
5. User may **Resume plan** (retry pending/failed) or **Rollback plan** (compensate done writes)

**Resume** (`resumePlanExecution`): skips `done` steps; resets `failed`/`skipped` to `pending`.

**Rollback** (`rollbackPlanExecution`): `compensateCompletedWrites` from stored `last_result`, clears plan.

---

## 6. Structured outputs

Использовать provider **json_schema** / **strict** response format где доступно для:
- `ExecutionPlan`
- `ClarificationRequest`
- **Не** для финального свободного ответа пользователю (может быть markdown)

---

## 7. Provider errors & retry

| Error | Retry (реализация `UOpenAICompatProvider`) |
|-------|------------------------------------------|
| HTTP 408 / 429 / 5xx | 1 retry после 400–500 ms |
| Transport error | 1 retry |
| Invalid tool JSON from model | validation error в `tool_result` (gateway) |

---

## 8. Сценарий C (реализовано)

Workflow state machine:

```
Idle -> Planning -> AwaitingConfirmation -> Executing -> Completed
                              |                |
                              v                v
                    Paused (checkpoint)    Rollback (compensate)
```

API: `confirmPlanExecution`, `resumePlanExecution`, `rollbackPlanExecution`. См. [MVP-Roadmap.md](MVP-Roadmap.md).

---

## 9. Threading

- Один `handleUserMessage` — worker (`QtConcurrent`) per GUI send
- `m_session_busy` — второй запрос с тем же `session_id` отклоняется с ошибкой «Session busy»
