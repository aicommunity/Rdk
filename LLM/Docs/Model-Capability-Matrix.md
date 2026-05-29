# Model capability matrix

| Tier | Role | Default profile | Notes |
|------|------|-----------------|-------|
| Router | intent / tool routing | `ollama-local` | Fast 7B acceptable |
| Cortex | main ReAct loop | `ollama-local` | **qwen2.5:14b** default |
| Utility | summarize / JSON | `ollama-local` | Same host, smaller calls |

## Ollama

- Set `OLLAMA_NUM_CTX` ≥ 8192 for 14B tool-calling runs.
- Lab profile may override model in settings.

## Feature flags

- `NMSDK_LLM_INPUT_ENSEMBLE` — input understanding ensemble
- `NMSDK_LLM_TASK_PATH_STRICT` — legacy fast-path for CI
