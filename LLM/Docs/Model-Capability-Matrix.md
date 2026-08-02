# Model capability matrix

## RU

| Tier | Role | Default profile | Notes |
|------|------|-----------------|-------|
| Router | intent / tool routing | `ollama-lite` | Fast 7B; **think OFF** |
| Cortex | main ReAct loop | `ollama-thinking` | **qwen3:14b** + `think:true` by default |
| Utility | summarize / JSON | active / lite | **think OFF** |

## Ollama thinking

- Cortex sends `think: true` when `LLM/enable_ollama_thinking` is true and provider `supports_thinking`.
- Env: `NMSDK_LLM_OLLAMA_THINKING_MODEL` (default `qwen3:14b`), lab host via `NMSDK_LLM_OLLAMA_BASE_URL` / settings.
- Preserve `message.thinking` on assistant tool-call turns for multi-step tool loops.
- Set `OLLAMA_NUM_CTX` ≥ 8192 for tool-calling + thinking runs.
- GUI: collapsible Reasoning bubble (TD-143).

## Feature flags

- `NMSDK_LLM_INPUT_ENSEMBLE` — input understanding ensemble
- `NMSDK_LLM_TASK_PATH_STRICT` — legacy fast-path for CI
- `LLM/enable_ollama_thinking` — Cortex thinking-first (default on)
- `NMSDK_LLM_CAPABILITY_RISK_TAU` — Capability Risk Gate threshold (default `0.55`); see [Weak-Model-Mitigation.md](Weak-Model-Mitigation.md)
- `NMSDK_LLM_CAPABILITY_RISK_DISABLE=1` — disable CRG pre-escalate / cascade / abstain

---

## EN

| Tier | Role | Default profile | Notes |
|------|------|-----------------|-------|
| Router | intent / tool routing | `ollama-lite` | Fast 7B; **think OFF** |
| Cortex | main ReAct loop | `ollama-thinking` | **qwen3:14b** + `think:true` by default |
| Utility | summarize / JSON | active / lite | **think OFF** |

## Ollama thinking

- Cortex sends `think: true` when `LLM/enable_ollama_thinking` is true and provider `supports_thinking`.
- Env: `NMSDK_LLM_OLLAMA_THINKING_MODEL` (default `qwen3:14b`), lab host via `NMSDK_LLM_OLLAMA_BASE_URL` / settings.
- Preserve `message.thinking` on assistant tool-call turns for multi-step tool loops.
- Set `OLLAMA_NUM_CTX` ≥ 8192 for tool-calling + thinking runs.
- GUI: collapsible Reasoning bubble (TD-143).

## Feature flags

- `NMSDK_LLM_INPUT_ENSEMBLE` — input understanding ensemble
- `NMSDK_LLM_TASK_PATH_STRICT` — legacy fast-path for CI
- `LLM/enable_ollama_thinking` — Cortex thinking-first (default on)
- `NMSDK_LLM_CAPABILITY_RISK_TAU` — Capability Risk Gate threshold (default `0.55`); see [Weak-Model-Mitigation.md](Weak-Model-Mitigation.md)
- `NMSDK_LLM_CAPABILITY_RISK_DISABLE=1` — disable CRG pre-escalate / cascade / abstain
