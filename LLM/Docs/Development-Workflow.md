# Development Workflow — Commits and Tech Debt

## RU

Mandatory process for **all agents and developers** implementing RDK LLM (phases 0–4).

---

## EN

## 1. Phase completion checklist

Execute **in order** at the end of each phase:

1. **Build** — default config passes:
   - `cmake -B build -DRDK_USE_LLM=OFF` → build NeuroModeler (or project default target)
   - If phase touches LLM: also `cmake -B build-llm -DRDK_USE_LLM=ON` → build `rdk.llm.core` + NeuroModeler
2. **Tests** — run tests introduced in this phase (see [Testing-Strategy.md](Testing-Strategy.md))
3. **Tech debt review** — open [TECH-DEBT.md](../TECH-DEBT.md):
   - Try to resolve every `open` item with `Target phase` ≤ current phase and `can_resolve_now` in Notes (or P0/P1 for this phase)
   - Add **new** rows for shortcuts taken this phase
   - Move resolved rows to **Done** with commit hash
4. **Docs** — update MVP checkboxes in [MVP-Roadmap.md](MVP-Roadmap.md) if criteria met; sync [Developer-Architecture.md](Developer-Architecture.md) / [Extension-Guide.md](Extension-Guide.md) when architecture or extension patterns change
5. **Rdk commit** — **one commit per phase/PR** in `Rdk/` (English message, see §2). Agent creates commit **without waiting for user prompt** when phase checklist is complete.
6. **Nmsdk bump commit** — update the root repository (`Nmsdk/`) to point at the new `Rdk` commit (subrepo/submodule pointer), then create **one English commit** in the root repo.

If build fails: **do not commit**; fix or document blocker in TECH-DEBT as P0 `blocked`.

---
