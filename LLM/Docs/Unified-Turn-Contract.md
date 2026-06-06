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

## 2. Workflow phases

| Phase | Meaning | Write tools |
|-------|---------|-------------|
| `Idle` | No turn in progress | — |
| `Running` | Turn accepted | policy |
| `Understanding` | Low-confidence explore (read-only budget) | denied by policy |
| `AwaitingUserInput` | `ask_user` issued; waiting for user | — |
| `Planning` | `propose_plan` / plan preview | denied |
| `AwaitingConfirmation` | HITL pending | — |
| `Executing` | ReAct tool loop | policy |
| `TaskExecuting` | Deterministic task executor (strict fast-path only) | gateway |
| `Completed` / `Failed` | Terminal | — |

Transitions: see `ULLMWorkflowState.h` (`workflowTransitionAllowed`).

---

## 3. Turn pipeline (normative)

```
handleUserMessage(envelope)
  → session busy guard
  → append user message, audit
  → if pending_user_question: resumeAskUser (skip intent/quantity re-parse)
  → else if pending_tool_arguments (disambiguation): resumePendingToolArgs
  → normalize → quantity → lifecycle hint (soft)
  → input understanding (ensemble)
  → task planner hint (optional fast-path if TaskPathMode::FastPath)
  → build tool exposure + context acquisition + ReAct loop
  → terminal: Completed | Failed | AwaitingUserInput | AwaitingConfirmation | pending_plan
```

---

## 4. Task path modes

| Mode | Behavior |
|------|----------|
| `HintOnly` | **Default.** `buildTaskPlan` → ephemeral hint; on failure → `task_plan_fallback_to_agent` audit + ReAct |
| `FastPath` | Legacy: execute task plan without LLM when plan builds (CI: `NMSDK_LLM_TASK_PATH_STRICT=1`) |

Settings: `LLM/task_path_mode`, env `NMSDK_LLM_TASK_PATH_STRICT=1`.

---

## 5. Clarification protocol

| Mechanism | When |
|-----------|------|
| `ask_user` tool | Model asks; `PendingUserQuestion` in store v3 |
| Gateway `needs_clarification` | Convert to in-loop clarify when `NMSDK_LLM_CLARIFY_IN_LOOP=1` |
| Legacy `returnDisambiguationRequest` | Deprecated for new paths; migrate to `ask_user` |

Resume turns **must not** re-run `resolveUserQuantity` or keyword intent when picking list index `1..N`.

---

## 6. Terminal outcomes

`TurnTerminal` (metadata on `LLMFinalResponse`): `Completed`, `MaxRounds`, `Cancelled`, `ProviderError`, `AwaitingUser`, `AwaitingConfirm`, `TaskFastPathCompleted`.

Every `tool_use` receives a matching `tool_result` (including cancel synthetic errors).

---

## 7. Invariants (unchanged)

- All mutations via `ULLMToolGateway`.
- Plans require explicit user **Run plan** (not auto-apply).
- No direct engine access from provider output.

---

## 8. Feature flags (agent v2)

| Flag / setting | Default (target) |
|----------------|------------------|
| `NMSDK_LLM_UNIFIED_TURN` | on |
| `NMSDK_LLM_CLARIFY_IN_LOOP` | on |
| `NMSDK_LLM_TASK_PATH_STRICT` | off (CI only) |
| `NMSDK_LLM_INPUT_ENSEMBLE` | on (settings) |

---

## 9. Version

| Field | Value |
|-------|-------|
| Contract | `unified-turn-contract-1.0` |
| Store schema | v3 (`pending_user_question`, `known_facts`) |
