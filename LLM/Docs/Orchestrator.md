# LLM Orchestrator

## RU

Normative flow: [Developer-Architecture.md](Developer-Architecture.md) §4.

## 1. `ULLMAgentOrchestrator`

Agent v2: единый turn pipeline, workflow phases, `TurnTerminal` на `LLMFinalResponse` (`Completed`, `Cancelled`, `AwaitingUser`, …). См. [Unified-Turn-Contract.md](Unified-Turn-Contract.md).

Audit: `tool_loop_entered` при входе в обработку `tool_calls`; `model_route_selected` для tier Router/Cortex (`ULLMModelRouter`).

Lifecycle: soft hints через ephemeral context; pre-LLM direct и forced `tool_choice` — только при env (`NMSDK_LLM_LIFECYCLE_DIRECT`, `NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE`).

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

## EN

## 2. `LLMRequestEnvelope`

```cpp
struct LLMRequestEnvelope {
    std::string session_id;
    std::string trace_id;
    std::string user_text;
    LLMSessionContext session;
    LLMProviderProfile provider_profile;
    LLMGuiContextSnapshot gui;  // portable snapshot from Qt LLMGuiContext
};
```

Ephemeral system blocks (manifest, GUI focus, retriever summary, agent notes, optional RAG prefetch) are assembled by `ULLMContextAssembler` each provider round — not stored in `ConversationState.messages` (except bootstrap / compaction summary).

---

## 3. Intent layer (`ULLMIntentParser`)

**Phase 1:** rule-based + keywords (fast, deterministic):

| Pattern | Intent |
|---------|--------|
| add, create, delete, save, load | Mutate |
| what, show, list, describe, which | Query |
| why, explain | Explain |

**Phase 3 (optionally):** small LLM call structured JSON `{ "intent": "query" }`.

`Auto` → rule-based; when uncertain → `Query` (safer).

---
