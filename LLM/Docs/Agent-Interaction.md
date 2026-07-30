# Agent interaction (v2)

## RU

Normative UX and protocol for free dialogue, clarification, and plans.

See [Unified-Turn-Contract.md](Unified-Turn-Contract.md).

## Clarification

- Prefer `ask_user` tool → `AwaitingUserInput` workflow phase.
- GUI shows question text and numbered choices when provided.
- Resume turns with list picks skip quantity/intent re-parse (`skip_pre_llm_funnel`).

## Plans

- `propose_plan` tool → preview in assistant panel → user **Run plan** (HITL).
- Task executor fast-path only when `NMSDK_LLM_TASK_PATH_STRICT=1`.

## Thinking / Reasoning (Cortex)

- Cortex default profile: `ollama-thinking`, model **`qwen3:14b`** (`NMSDK_LLM_OLLAMA_THINKING_MODEL` override).
- Setting `LLM/enable_ollama_thinking` (default on): send `think:true`; keep reasoning out of the answer bubble.
- GUI: collapsible **Reasoning** block (`<details>`) from streamed `on_thinking_token` and/or `final.thinking` (truncated ~8KB). Same pattern as tool traces.
- Status line may show “Model is reasoning…” while thinking tokens arrive.
- Do not dump raw CoT into the assistant answer text.

## Settings

| Key | Default |
|-----|---------|
| `LLM/task_path_mode` | `hint_only` (env strict for CI) |
| `LLM/enable_ollama_thinking` | on (Cortex thinking-first) |
| `NMSDK_LLM_INPUT_ENSEMBLE` | on |
| `NMSDK_LLM_CLARIFY_IN_LOOP` | on |

---

## EN

Normative UX and protocol for free dialogue, clarification, and plans.

See [Unified-Turn-Contract.md](Unified-Turn-Contract.md).

## Clarification

- Prefer `ask_user` tool → `AwaitingUserInput` workflow phase.
- GUI shows question text and numbered choices when provided.
- Resume turns with list picks skip quantity/intent re-parse (`skip_pre_llm_funnel`).

## Plans

- `propose_plan` tool → preview in assistant panel → user **Run plan** (HITL).
- Task executor fast-path only when `NMSDK_LLM_TASK_PATH_STRICT=1`.

## Thinking / Reasoning (Cortex)

- Cortex default profile: `ollama-thinking`, model **`qwen3:14b`** (`NMSDK_LLM_OLLAMA_THINKING_MODEL` override).
- Setting `LLM/enable_ollama_thinking` (default on): send `think:true`; keep reasoning out of the answer bubble.
- GUI: collapsible **Reasoning** block (`<details>`) from streamed `on_thinking_token` and/or `final.thinking` (truncated ~8KB). Same pattern as tool traces.
- Status line may show “Model is reasoning…” while thinking tokens arrive.
- Do not dump raw CoT into the assistant answer text.

## Settings

| Key | Default |
|-----|---------|
| `LLM/task_path_mode` | `hint_only` (env strict for CI) |
| `LLM/enable_ollama_thinking` | on (Cortex thinking-first) |
| `NMSDK_LLM_INPUT_ENSEMBLE` | on |
| `NMSDK_LLM_CLARIFY_IN_LOOP` | on |
