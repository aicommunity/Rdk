# Capability Packs (Agent Spine)

## RU

Нормативный контракт единицы расширяемости LLM-агента. Реализация: `Rdk/LLM/Core/Packs/`, turn pipeline: `Rdk/LLM/Core/Orchestrator/Turn/`.

См. также: [Developer-Architecture.md](Developer-Architecture.md), [Unified-Turn-Contract.md](Unified-Turn-Contract.md), [Extension-Guide.md](Extension-Guide.md), [TECH-DEBT.md](../TECH-DEBT.md) (DD-PACK-001, TD-158+).

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
| `>= 0.85` | If pack implements `tryRecorded` → Recorded short-circuit |
| `0.4 .. 0.85` | Hints + tool allowlist merge only |
| `< 0.4` | Ignore |

Audit events: `pack_matched`, `pack_recorded_fired` (`pack_id`, `score`).

## 3. Builtin packs (migration)

| Pack id | Source DD | Status |
|---------|-----------|--------|
| `channel_calc` | DD-CALC-001 | Phase A proof |
| `component_structure` | DD-STRUCT-001 | Phase B |
| `watch_plot` | DD-WATCH-001/002 | Phase B |
| `connect` | DD-CONN-001/002 | Phase B |
| `add_component_direct` | DD-MEM-002/003 | Phase B |
| `lifecycle_soft` | TD-102 | Phase B |

## 4. Registration

```cpp
// LLMServices::initialize — after Register*Tools
packs.registerPack(std::make_unique<UPackChannelCalc>());
```

Libraries may register packs from `Libraries/*/Llm/` (phase F).

## 5. MCP

Out of scope (DD-PACK-002). Revisit on product request for out-of-process tools.

## 6. Working goals

See DD-WM-001 / TD-162: structured `working_goals` on `ConversationState` (store v4), injected as ephemeral each provider round.
