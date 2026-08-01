# Developer architecture — RDK LLM assistant

## RU

Normative overview for implementers and coding agents. Contract details remain in topic-specific docs; this file is the **single source of truth** for structure and request flow.

**See also:** [Extension-Guide.md](Extension-Guide.md) (how to add tools/knowledge), [Architecture.md](Architecture.md) (short index + CMake), [Post-MVP-Implementation-Plan.md](Post-MVP-Implementation-Plan.md) (deferred features).

---

## EN

## 1. Role of the LLM

The LLM is a **planner and intent interpreter**, not the source of truth and not a direct executor.

```
User → UI → Orchestrator → ILLMProvider
              ↓
        Tool Gateway → Policy
              ↓
        URdkApplicationCommands → UApplication   (configuration lifecycle)
        URdkDomainAccess        → UEngine/UNet   (graph read/write)
              ↓
        ILLMPresentationSink (shell refresh + GUI-thread host commands via `LLMPresentationEvent`)
              ↓
        ILLMProjectContextProvider + UDocSearchIndex (NMSDK knowledge)
```

The model must **never**:

- Apply SQL/XML or raw project files bypassing the gateway.
- Call `UEngine` / `UNet` / `UApplication` directly from provider output.
- Escalate permissions beyond `ULLMPolicyEngine` and session flags.

**Invariants** (full list): [README.md](README.md) § strict invariants.

---

## 2. TurnPipeline + Capability Packs

Turn entry is `ULLMTurnPipeline` (`Core/Orchestrator/Turn/`) with `ITurnPhase` phases; the current body is still largely `ULLMTurnPhaseLegacy` (strangler toward thinner phases — TD-170). Domain scenarios live in **Capability Packs** (`Core/Packs/`), not in orchestrator if-ladders (DD-PACK-001).

**Per-turn shape:**

1. Packs `rank` → optional Recorded short-circuit via `tryRecordedCapabilityPacks` (must use `recordedToolInvoke`).
2. Else ReAct: pack hints (`collectPackHintsMarkdown`) prepended as ephemeral `## Capability pack hints`; `extra_tool_names` merged into the tool filter.
3. `working_goals` on `ConversationState` (store v4, DD-WM-001) seeded from high-score packs and injected each provider round.

**Multi-goal (DD-PACK-003):** when ≥2 packs score ≥0.85 **or** the utterance is a compound conjunction (e.g. add + calc), Recorded runs **sequentially** in dependency order (`add_component_direct` → … → `channel_calc`) without exclusive first-hit short-circuit. HITL/clarify pauses the chain.

**ADR note:** lifecycle **load** direct invoke stays env-gated in the orchestrator (`NMSDK_LLM_LIFECYCLE_DIRECT=1`, DD-MEM-002 / TD-102); `lifecycle_soft` pack is hints-only — not migrating load into packs.

Details: [Capability-Packs.md](Capability-Packs.md), [Unified-Turn-Contract.md](Unified-Turn-Contract.md) § Recorded bypass.