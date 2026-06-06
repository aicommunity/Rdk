# Post-Audit Phase 2 — Implementation Plan (agent-ready)

## RU

**Audience:** Cursor / CI agents implementing the next LLM hardening sprint after the assistant audit (dialog + autonomy).

**Baseline:** Audit burn-down complete; all rows TD-068 … TD-071 are `done` in [TECH-DEBT.md](../TECH-DEBT.md). Normative architecture: [Developer-Architecture.md](Developer-Architecture.md). Process: [Development-Workflow.md](Development-Workflow.md).

**Do not edit:** `.cursor/plans/llm_assistant_audit_*.plan.md` (historical audit only).

---

## EN

## Как use (для human)

1. Copy block **Agent bootstrap prompt** (§9) в new чат agent.
2. Agent executes phases **strictly по order** (0 → 6); phase 7 — optionally.
3. After each phases — checklist из [Development-Workflow.md](Development-Workflow.md) + commit в `Rdk/` + bump submodule в `Nmsdk/`.
4. Не commitить `LLM/index/*` в root без explicit request user.

---
