# Unified Turn Contract (Agent v2)

## RU

Normative contract for the LLM assistant turn pipeline. Implementation: `ULLMUnifiedTurnController`, `ULLMAgentOrchestrator` (delegate). Deferred ADR entries: [TECH-DEBT.md](../TECH-DEBT.md) § Deferred decisions log.

**See also:** [Developer-Architecture.md](Developer-Architecture.md), [Orchestrator.md](Orchestrator.md), [Agent-Interaction.md](Agent-Interaction.md), [Conversation-State.md](Conversation-State.md).

---

## EN

## 1. Goals

1. **Single spine:** every user message enters a ReAct loop unless resuming HITL/clarify-only state **or** taking an allowlisted Recorded bypass (below).
2. **Deterministic planners are hints**, not hard bypasses (task/connect planners → ephemeral blocks; fast-path opt-in **and** must use RecordedToolInvoke).
3. **Clarification is a protocol** (`ask_user`, `AwaitingUserInput`, pending merge without full re-parse).
4. **Intent is a soft prior**; writes gated by policy, not by hiding tools from the API.

## 2. Recorded bypass (DD-MEM-001 / TD-150)

Any tool invoke that skips the LLM tool-loop must use `recordedToolInvoke` (`ULLMRecordedToolInvoke`) so the conversation store gets the same memory contract as ReAct:

1. Assistant message with `assistant_tool_calls` (stable `tool_call_id`).
2. On completion: Tool message with the same `tool_call_id`.
3. On HITL pending: assistant tool_calls only; store `PendingConfirmation.tool_call_id`; Tool message on `confirmPending`.
4. On successful write: `recordWriteToolOutcome` → `session_graph` (`last_add`, links, …).

### Direct allowlist (DD-MEM-002)

| Path | When |
|------|------|
| `add_component` | Explicit registered class in user text, **or** repeat-cue («ещё/таких же») with valid `session_graph.last_add` |
| `open_recent_configuration` | Index / pending open_recent (TD-132) |
| Pending-arg resume | Clarification / lifecycle args complete |
| `load_configuration` | Only if `NMSDK_LLM_LIFECYCLE_DIRECT=1` (TD-102) |

Do **not** expand cue-direct to delete/connect without a new ADR. `last_add` is a session side-channel (ephemeral inject), not a substitute for transcript.

Task FastPath / PlanExecutor: HintOnly default; when FastPath or plan Run executes steps, each step goes through RecordedToolInvoke.
