# Post-MVP implementation plan

## RU

Roadmap for **deferred code** after MVP / write-tools P1–P3. Architecture baseline: [Developer-Architecture.md](Developer-Architecture.md).

**Not in scope here:** documentation-only changes (see Development-Workflow). **Forbidden:** unbounded autonomous agent (Anti-Patterns #11).

---

## EN

## 1. Inventory

| ID | Topic | Current status | Priority |
|----|-------|----------------|----------|
| TD-041 | Manual GUI write-tools walkthrough | open | P0 — QA |
| **Scenario D** | Configurable autonomous / semi-autonomous agent | implemented (strict/semi-auto/task path) | P1 |
| TD-035 | Dedicated `tool_disambiguation` UI | done (`needs_tool_disambiguation` + dock flow) | P2 |
| TD-036 | Embedding-based dynamic tool routing | cancelled MVP → revisit | P3 |
| TD-031 | YAML knowledge manifest | cancelled MVP → revisit | P3 |
| TD-034 | Function-level source chunking (ctags) | cancelled MVP → revisit | P3 |

Cancelled MVP items are **not rejected forever**; each needs a new TECH-DEBT row when work starts.

---
