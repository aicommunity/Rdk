# Unified Turn Contract (Agent v2)

## RU

Normative contract for the LLM assistant turn pipeline. Implementation: `ULLMUnifiedTurnController`, `ULLMAgentOrchestrator` (delegate). Deferred ADR entries: [TECH-DEBT.md](../TECH-DEBT.md) § Deferred decisions log.

**See also:** [Developer-Architecture.md](Developer-Architecture.md), [Orchestrator.md](Orchestrator.md), [Agent-Interaction.md](Agent-Interaction.md).

---

## EN

## 1. Goals

1. **Single spine:** every user message enters a ReAct loop unless resuming HITL/clarify-only state.
2. **Deterministic planners are hints**, not hard bypasses (task/connect planners → ephemeral blocks; fast-path opt-in).
3. **Clarification is a protocol** (`ask_user`, `AwaitingUserInput`, pending merge without full re-parse).
4. **Intent is a soft prior**; writes gated by policy, not by hiding tools from the API.

---
