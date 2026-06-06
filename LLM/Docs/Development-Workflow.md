# Development Workflow — Commits and Tech Debt

Mandatory process for **all agents and developers** implementing RDK LLM (phases 0–4).

---

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

## 2. Commit conventions (English only)

**Format (Conventional Commits):**

```
<type>(rdk-llm): <short summary>

[optional body: what changed, why, TECH-DEBT IDs closed]

[optional: Breaking changes, Refs TD-00x]
```

**Types:** `feat`, `fix`, `docs`, `build`, `test`, `refactor`, `chore`

### Required commits by phase

| Phase | When | Example subject |
|-------|------|-----------------|
| **0** | CMake + stubs + registry skeleton; Pending-CMake applied | `build(rdk-llm): add optional RDK_USE_LLM target and core stubs` |
| **0-docs** | *(already done if only docs)* | `docs(rdk-llm): add LLM design documentation package` |
| **1** | Read tools + orchestrator + Ollama + GUI chat (no write) | `feat(rdk-llm): read-only tools, orchestrator, and assistant dock` |
| **2** | Write tools + policy + HITL preview + NMSDK context | `feat(rdk-llm): write tools with policy, HITL, and NMSDK context` |
| **3** | Tests, audit hardening, CI llm job | `test(rdk-llm): add LLM unit tests, audit log, and CI matrix` |
| **4** | Embedded llama provider | `feat(rdk-llm): add embedded llama.cpp provider target` |

### LLM-first architecture (PR0a–PR5)

| PR | Example subject |
|----|-----------------|
| **PR1** | `fix(rdk-llm): LLM-first lifecycle path for RU project requests` |
| **PR0a** | `feat(rdk-llm): builtin knowledge catalog and docs/sources index` |
| **PR0b** | `feat(rdk-llm): prebuilt knowledge index pack under Bin/LLM/index` |
| **PR2** | `feat(rdk-llm): agent manifest, tool filter, and ReAct termination` |
| **PR3** | `feat(rdk-llm): localized user-facing LLM system messages (ru/en)` |
| **PR4** | `feat(rdk-llm): query intent path and retrieval score threshold` |
| **PR5** | `test(rdk-llm): retrieval fixtures, agent docs, and E2E RU scenarios` |

**Sub-commits within a phase** are allowed only if logically separate and still **before** phase is marked done — e.g. `build(rdk-llm): wire NeuroModeler to rdk.llm.core` then `feat(rdk-llm): implement get_net_snapshot tool`. Prefer **squashing** to one commit per phase before starting next phase (user may request otherwise).

### Commit body template

```
Phase N: <title>

Deliverables:
- ...

Build: RDK_USE_LLM=OFF OK; RDK_USE_LLM=ON OK (Linux/Win as applicable)

Tech debt:
- Closed: TD-00x, TD-00y
- Added: TD-00z (P2, target phase N+1)

Docs: MVP-Roadmap phase N criteria updated
```

---

## 3. TECH-DEBT.md workflow

**File:** [`Rdk/LLM/TECH-DEBT.md`](../TECH-DEBT.md)

### When to add a row

- «Сделаем потом» в коде или review
- Known limitation documented as acceptable for this phase
- External dependency (vcpkg, Ollama install, GPU)
- Design alternative rejected but worth revisiting

### Deferred decisions (ADR-style, required when applicable)

If you defer a decision that changes the architecture or affects future implementation work, record it in the **Deferred decisions log** section of `TECH-DEBT.md` with:

- what was decided (and what was not),
- why it was deferred,
- the phase by which to revisit, and
- a concrete trigger (TD id, env flag, failing scenario, or milestone).

### Row template

```markdown
| TD-0NN | Short title | phase deferred | target phase | P1 | open | Details; optional "can_resolve_now" |
```

### Phase-end debt pass (required)

```
FOR each open item WHERE target_phase <= current_phase:
    IF effort <= ~2h AND no blocker:
        implement → Status=done → mention in commit
    ELSE IF scope creep:
        bump target_phase OR raise Priority
AT phase end:
    FOR each new shortcut in diff:
        add TD row
```

**Do not** close P0 items by lowering priority — fix or `blocked` with reason.

---

## 4. Git safety (project rules)

- **Never** `git push --force` to main/master unless user explicitly asks
- **Never** amend unless: user asked, OR last commit is yours unpushed and hook fixed files only
- **Never** `--no-verify` unless user asks
- **Do not** commit secrets (API keys, `.env`)
- Commit **only** RDK LLM-related paths unless phase explicitly touches NeuroModeler/CMake root

---

## 5. Agent instructions (copy-paste)

At start of each phase:

1. Read [AGENTS.md](../AGENTS.md), relevant phase doc, [TECH-DEBT.md](../TECH-DEBT.md)
2. Implement phase scope only
3. Run phase completion checklist (§1)
4. `git add` relevant files → commit with §2 message
5. Report: commit hash, closed TD IDs, new TD IDs, MVP checklist state

---

## 6. CI alignment

| Phase | CI expectation |
|-------|----------------|
| 0+ | `default` job: `RDK_USE_LLM=OFF` green |
| 1+ | Add `llm-linux` job (document in Build.md when added) |
| 4 | Add `llm-embedded` optional job |

Record CI gaps as TECH-DEBT until job exists.
