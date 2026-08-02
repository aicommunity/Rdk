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
5. **Act-or-Clarify (DD-ACT-001 / TD-153):** when tools were offered and the goal is actionable (Mutate/Plan writes, connect/add/lifecycle detectors, **or Query/Explain without successful read evidence yet**), an empty `tool_calls` round must not finalize as free prose — recovery → tool / `ask_user` / `NO_SUITABLE_TOOL`. After a **successful read-only tool trace** (e.g. `get_net_snapshot` ok, no writes), prose is allowed for Query/Explain and for Mutate false-positives — **except** when a capability pack sets `force_include_write` (e.g. `project_description`): then prose stays blocked until `update_configuration` succeeds, including on **exhaust soft-fallback** (chat 18-19-54: do not show model greetings). A second description-write recovery round may run; if still no write, use a deterministic user_lang message. **Failed-only** tool traces do **not** unlock prose. Does not force API `tool_choice` under thinking (DD-THINK-003); enforcement is post-completion in the orchestrator loop.
6. **Live analogous connect (DD-CONN-002 / TD-152):** when ConnectPlan builds with `analogous_ref_token`, execute (or confirm) even under HintOnly — ReAct essay is not the primary path.

## 2. Recorded bypass (DD-MEM-001 / TD-150)

Any tool invoke that skips the LLM tool-loop must use `recordedToolInvoke` (`ULLMRecordedToolInvoke`) so the conversation store gets the same memory contract as ReAct:

1. Assistant message with `assistant_tool_calls` (stable `tool_call_id`).
2. On completion: Tool message with the same `tool_call_id`.
3. On HITL pending: assistant tool_calls only; store `PendingConfirmation.tool_call_id`; Tool message on `confirmPending`.
4. On successful write: `recordWriteToolOutcome` → `session_graph` (`last_add`, links, …).

### Direct allowlist (DD-MEM-002)

| Path | When |
|------|------|
| `add_component` | Explicit registered class in user text, **or** continuity cue (ещё/таких же/continuer noun нейрон|компонент|блок|модуль) with valid `session_graph.last_add` (DD-MEM-003) |
| `open_recent_configuration` | Index / pending open_recent (TD-132) |
| Pending-arg resume | Clarification / lifecycle args complete |
| `load_configuration` | Only if `NMSDK_LLM_LIFECYCLE_DIRECT=1` (TD-102) |

Do **not** expand cue-direct to delete/connect without a new ADR. `last_add` is a session side-channel (ephemeral inject), not a substitute for transcript.

Task FastPath / PlanExecutor: HintOnly default; when FastPath or plan Run executes steps, each step goes through RecordedToolInvoke.

**FastPath success must be user-visible:** `ULLMTaskExecutor` summary lists created links/adds (not only a step count); each successful step is recorded on `ConversationState::current_turn_tool_trace` so chat/AiChats HTML get a Tools `<details>` block with 4-tuple args (parity with direct `add_component`). Do not invent LLM Reasoning for pure FastPath turns — archive full thinking only when the model actually produced it (soft ~256KB safety cap, not 8KB).

**Channel calc FastPath (DD-CALC-001):** phrases like «запусти расчет» / `start calculation` (also pause/reset/step) invoke `*_channel_calculation` via `recordedToolInvoke` **before** TaskPath/ReAct. Autonomous whitelist includes these tools plus `ask_user` (Act-or-Clarify clarification must not be denied). Outcome and Tools block are user-visible; missing open config returns the tool error, not an essay.

**Structure knowledge gate + dendrite FastPath (DD-STRUCT-001):** structure goals (`дендрит` / `NumSoma` / …) force Mutate, prefetch docs, ephemeral Pulse canon hint, and Act-or-Clarify recovery (`set_property` + `calculate_component`, no `add_component` Dendrite). Parsed «N дендритов длинами …» runs FastPath: `StructureBuildMode=2`, `NumSomaMembraneParts`, `NumDendriteMembranePartsVec` (space-separated), `calculate_component`. Fuzzy `property_name` covers typos like `numDendridetMembranePartsVec`.

**Watch plot FastPath (DD-WATCH-001 / DD-WATCH-002):** «на график …» → `add_watch_series` (optional `create_watch_mdi`). Nested roles («ltzone», «низкопороговая зона») resolve to dotted `Parent.Child` via `resolveNestedWatchTarget` — never parent-only `Output` when nested_hint is set. Autonomous allows UI/watch tools without burning write-step budget. Chat input Tab-completes model long_names / ClassesList / properties.

### Recorded via Capability Packs (DD-PACK-001 / DD-PACK-003)

Primary Recorded routing is **pack-owned**: `tryRecordedCapabilityPacks` ranks packs, then:

| Case | Behavior |
|------|----------|
| Single high match (`>= 0.85`) | That pack's `tryRecorded` may short-circuit the turn |
| Multi-goal: ≥2 high matches **or** compound conjunction (DD-PACK-003) | Run high packs **sequentially** in dependency order (`add_component_direct` → structure/watch/connect → `channel_calc`); no exclusive first-hit abort. Clarification/HITL pauses remaining packs |
| Score `0.4 .. 0.85` (or high match that did not Record) | Continue to ReAct with pack ephemeral hints + `extra_tool_names` allowlist merge |

`working_goals` (DD-WM-001) are seeded for high packs and updated from tool evidence. Lifecycle **load** remains orchestrator env-gated (`NMSDK_LLM_LIFECYCLE_DIRECT=1`); not pack-migrated.
