# Model capability matrix

## RU

| Tier | Role | Default profile | Notes |
|------|------|-----------------|-------|
| Router | intent / tool routing | `ollama-lite` | Fast 7B; **think OFF** |
| Cortex | main ReAct loop | `ollama-thinking` | **qwen3** + `think:true` by default |
| Utility | summarize / JSON | active / lite | **think OFF** |

## Ollama thinking

- Cortex sends `think: true` when `LLM/enable_ollama_thinking` is true and provider `supports_thinking`.
- Env: `NMSDK_LLM_OLLAMA_THINKING_MODEL` (default `qwen3`), lab host via `NMSDK_LLM_OLLAMA_BASE_URL` / settings.
- Preserve `message.thinking` on assistant tool-call turns for multi-step tool loops.
- Set `OLLAMA_NUM_CTX` ≥ 8192 for tool-calling + thinking runs.

## Feature flags

- `NMSDK_LLM_INPUT_ENSEMBLE` — input understanding ensemble
- `NMSDK_LLM_TASK_PATH_STRICT` — legacy fast-path for CI
- `LLM/enable_ollama_thinking` — Cortex thinking-first (default on)

---

## EN

| Tier | Role | Default profile | Notes |
|------|------|-----------------|-------|
| Router | intent / tool routing | `ollama-lite` | Fast 7B; **think OFF** |
| Cortex | main ReAct loop | `ollama-thinking` | **qwen3** + `think:true` by default |
| Utility | summarize / JSON | active / lite | **think OFF** |

## Ollama thinking

- Cortex sends `think: true` when `LLM/enable_ollama_thinking` is true and provider `supports_thinking`.
- Env: `NMSDK_LLM_OLLAMA_THINKING_MODEL` (default `qwen3`), lab host via `NMSDK_LLM_OLLAMA_BASE_URL` / settings.
- Preserve `message.thinking` on assistant tool-call turns for multi-step tool loops.
- Set `OLLAMA_NUM_CTX` ≥ 8192 for tool-calling + thinking runs.

## Feature flags

- `NMSDK_LLM_INPUT_ENSEMBLE` — input understanding ensemble
- `NMSDK_LLM_TASK_PATH_STRICT` — legacy fast-path for CI
- `LLM/enable_ollama_thinking` — Cortex thinking-first (default on)
