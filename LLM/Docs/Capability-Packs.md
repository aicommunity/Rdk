# Capability Packs (Agent Spine)

## RU

Нормативный контракт единицы расширяемости LLM-агента. Реализация: `Rdk/LLM/Core/Packs/`, turn pipeline: `Rdk/LLM/Core/Orchestrator/Turn/`.

См. также: [Developer-Architecture.md](Developer-Architecture.md), [Unified-Turn-Contract.md](Unified-Turn-Contract.md), [Extension-Guide.md](Extension-Guide.md), [TECH-DEBT.md](../TECH-DEBT.md) (DD-PACK-001, DD-PACK-003, TD-158+).

---

## EN

## 1. Goals

1. **Thin spine:** `ULLMTurnPipeline` + `ITurnPhase` own the turn; domain scenarios live in packs.
2. **Pack = extensibility unit** (Cursor Skills analogue): match, ephemeral hints, optional Recorded strategy, tool allowlist contribution, Act-or-Clarify recovery tools.
3. **No FastPath if-ladder** in `ULLMAgentOrchestrator` after phase B (DD-PACK-001).
4. **Recorded only via** `recordedToolInvoke` (DD-MEM-001).

## 2. Score thresholds (DD-PACK-001)

| Score | Behavior |
|-------|----------|
| `>= 0.85` | If pack implements `tryRecorded` → Recorded (single-goal short-circuit; multi-goal → §7) |
| `0.4 .. 0.85` | Hints + tool allowlist merge only |
| `< 0.4` | Ignore |

On ReAct continuation, orchestrator injects `## Capability pack hints` from `collectPackHintsMarkdown` and merges `collectPackExtraToolNames` into the tool filter (TD-167).

Audit events: `pack_matched`, `pack_recorded_fired` (`pack_id`, `score`; multi-goal adds `"multi": true`).

## 3. Builtin packs (migration)

| Pack id | Source DD | Status |
|---------|-----------|--------|
| `channel_calc` | DD-CALC-001 | Migrated (Recorded + hints) |
| `component_structure` | DD-STRUCT-001 | Migrated (Recorded + hints) |
| `watch_plot` | DD-WATCH-001/002 | Migrated (Recorded + hints) |
| `connect` | DD-CONN-001/002 | Hints only; live-analogous execute still in orchestrator (TD-169) |
| `add_component_direct` | DD-MEM-002/003 | Migrated (Recorded + hints) |
| `lifecycle_soft` | TD-102 | Hints only; env-gated load direct stays in orchestrator (not migrating) |

## 4. Registration

```cpp
// LLMServices::initialize — after Register*Tools
RegisterBuiltinCapabilityPacks(packs); // channel_calc, component_structure, watch_plot, …
```

Libraries may register packs from `Libraries/*/Llm/` (see [Extension-Guide.md](Extension-Guide.md) + `Core/Packs/Template/`). `Register*LibLlmTools` remain valid for tools; packs wrap domain scenarios.

## 5. MCP

Out of scope (DD-PACK-002). Revisit on product request for out-of-process tools.

## 6. Working goals

See DD-WM-001 / TD-162: structured `working_goals` on `ConversationState` (store v4), injected as ephemeral each provider round. High-score packs seed goals; tool outcomes update status via `workingGoalIdForToolName`.

## 7. Multi-goal Recorded (DD-PACK-003)

When **≥2 packs** score ≥0.85 **or** `isCompoundActionableGoal` (conjunction + multiple actionable families, e.g. «добавь … и запусти расчет»), Recorded does **not** exclusive-short-circuit on the first handled pack.

1. Sort high packs by fixed dependency order: mutate/structure/watch/connect before `channel_calc` side-effect.
2. Run each pack's `tryRecorded` sequentially; merge successful parts into one response.
3. Clarification / HITL / pending confirmation **pauses** the chain; remaining packs resume after the user answers.

Single high match without compound → unchanged first-pack short-circuit.