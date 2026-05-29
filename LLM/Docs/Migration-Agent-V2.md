# Migration guide — Agent v2

## Summary

Agent v2 introduces a unified turn spine, `ask_user` clarification, task-path hint-only default, and tiered tool exposure.

## Breaking behavior changes

1. **Task path:** mutate multi-step goals no longer hard-fail when the deterministic planner fails; they fall back to ReAct unless `NMSDK_LLM_TASK_PATH_STRICT=1`.
2. **Session store:** schema v3 adds `pending_user_question` and `known_facts`.
3. **Default model:** Ollama profiles default to `qwen2.5:14b`.

## Migration steps

1. Rebuild with `RDK_USE_LLM=ON`.
2. Run `ctest -R 'Test_LLM_'`.
3. Clear stale sessions under `LLM/sessions/` if resume errors occur.
4. Enable strict task path in CI only: `export NMSDK_LLM_TASK_PATH_STRICT=1`.

## Deferred decisions

See [TECH-DEBT.md](../TECH-DEBT.md) Deferred decisions log.
