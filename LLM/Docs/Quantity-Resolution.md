# Quantity resolution

The assistant resolves a **primary operation count** once per user turn (not per tool). Example: «добавь шесть нейронов» → `primary=6` for the whole turn.

## Pipeline

1. **Heuristic** — [`ULLMQuantityParser`](../Core/Orchestrator/ULLMQuantityParser.cpp): digits (bare or `N x` / `N раз`), number words 2–20 (ru/en).
2. **LLM fallback** (optional) — [`ULLMQuantityResolver`](../Core/Orchestrator/ULLMQuantityResolver.cpp): JSON `{"primary_count": N}` when heuristic misses but the text looks like a mutate/count request.
3. **Session** — result stored in `ConversationState.last_quantity` and reused by task/plan paths, pending repeat, and context hints.

## Follow-up disambiguation

When the user replies with a **class list index** (e.g. `1`) or exact class name while `pending_tool_arguments` is active, quantity is **not** re-resolved. `requested_repeat_count` on pending keeps the count from the original message.

## Plan step repeats

`ExecutionPlanStep.repeat_count` is applied only for tools in the repeat whitelist ([`ULLMPlanRepeatPolicy`](../Core/Orchestrator/ULLMPlanRepeatPolicy.cpp)); currently `add_component`.

## Environment

| Variable | Default | Meaning |
|----------|---------|---------|
| `NMSDK_LLM_QUANTITY_LLM` | `0` | Enable LLM fallback |
| `NMSDK_LLM_QUANTITY_MAX` | `50` | Upper bound for primary count |

Runtime setting: `LLMRuntimeProviderSettings.quantity_llm_fallback` (default off).

## Multi-quantity requests

Several counts in one utterance (e.g. «3 NSPGenerator and 2 UModel») are parsed by [`ULLMTaskPlanParsing`](../Core/Orchestrator/ULLMTaskPlanParsing.cpp) into **per-class** `add_component` plan steps. The session `last_quantity` primary count is **not** applied when the plan already has multiple add steps (avoids forcing every step to `max(2,3)`).

Compound goals use `goal_success` type `multi_component_count`. See TD-091 for richer plan/LLM paths.
