# Testing Strategy

## RU

## 1. Уровни (как у платформы, не как у чата)

| Уровень | Что | Где | LLM required |
|---------|-----|-----|--------------|
| L1 Schema | Argument validator | `Rdk/Tests/Unit/LLM/` | No |
| L2 Tool handler | Domain mock | Unit | No |
| L3 Gateway | Policy + idempotency | Unit | No |
| L4 Tool selection | Intent → filter | Unit | No |
| L5 Entity resolution | Candidates | Unit | No |
| L6 Orchestrator | Mock provider | Integration | No |
| L7 Regression phrases | Intent/rules smoke | Unit + fixtures | No |
| L7.5 Agent scenarios | JSON matrix + mock runner | `Test_LLM_AgentScenarios` | D: No / E2e: Yes |
| L7.5b Context scenarios | `deterministic/context/ctx_*.json` (5 cases) | same | No |
| L8 E2E / Manual | Real Ollama | Manual | Yes |

**CI default (`RDK_USE_LLM=OFF`):** LLM tests **не собираются**.

**CI `ci-llm-linux.sh`:** `cmake -DRDK_USE_LLM=ON` + ctest `Rdk/Tests/Unit/LLM` (44+ targets, incl.
`Test_LLM_RegressionFixtures`, `Test_LLM_KnowledgeCatalog`, `Test_LLM_WriteToolsAudit`, `Test_LLM_WriteToolsP1`,
`Test_LLM_WriteToolsEngine` (live engine + `test_valid`), `Test_LLM_DocRetrieval`).

**CI agentic-risk gate (`ci-llm-agentic-risk.sh`):** after full ctest, runs P0/P1 suites from
`Rdk/Tests/Fixtures/LLM/agentic-risk/manifest.json` via `--gtest_filter` (see
[Agentic-Risk-Test-Matrix.md](Agentic-Risk-Test-Matrix.md)). Invoked automatically at the end of
`Scripts/ci-llm-linux.sh`. Standalone:

```bash
cmake -S . -B build-llm-ci -DRDK_USE_LLM=ON -DBUILD_TESTING=ON
cmake --build build-llm-ci --target Test_LLM_SessionPersist Test_LLM_ToolGateway \
  Test_LLM_Orchestrator Test_LLM_ArgumentValidator -j"$(nproc)"
BUILD_DIR=build-llm-ci ./Scripts/ci-llm-agentic-risk.sh
```

**CI agent scenario matrix (`ci-llm-agent-scenarios.sh`):** after agentic-risk, runs
`Test_LLM_AgentScenarios` deterministic filter (required) and `E2e/*` (lab Ollama; cases
`GTEST_SKIP` when unreachable). See [Agent-Scenario-Matrix.md](Agent-Scenario-Matrix.md).

```bash
BUILD_DIR=build-llm-ci ./Scripts/ci-llm-agent-scenarios.sh
```

---

## EN

## 1. Levels (platform-style, not chat-style)

| Level | What | Where | LLM required |
|---------|-----|-----|--------------|
| L1 Schema | Argument validator | `Rdk/Tests/Unit/LLM/` | No |
| L2 Tool handler | Domain mock | Unit | No |
| L3 Gateway | Policy + idempotency | Unit | No |
| L4 Tool selection | Intent → filter | Unit | No |
| L5 Entity resolution | Candidates | Unit | No |
| L6 Orchestrator | Mock provider | Integration | No |
| L7 Regression phrases | Intent/rules smoke | Unit + fixtures | No |
| L7.5 Agent scenarios | JSON matrix + mock runner | `Test_LLM_AgentScenarios` | D: No / E2e: Yes |
| L7.5b Context scenarios | `deterministic/context/ctx_*.json` (5 cases) | same | No |
| L8 E2E / Manual | Real Ollama | Manual | Yes |

**CI default (`RDK_USE_LLM=OFF`):** LLM tests are **not built**.

**CI `ci-llm-linux.sh`:** `cmake -DRDK_USE_LLM=ON` + ctest `Rdk/Tests/Unit/LLM` (44+ targets, incl.
`Test_LLM_RegressionFixtures`, `Test_LLM_KnowledgeCatalog`, `Test_LLM_WriteToolsAudit`, `Test_LLM_WriteToolsP1`,
`Test_LLM_WriteToolsEngine` (live engine + `test_valid`), `Test_LLM_DocRetrieval`).

**CI agentic-risk gate (`ci-llm-agentic-risk.sh`):** after full ctest, runs P0/P1 suites from
`Rdk/Tests/Fixtures/LLM/agentic-risk/manifest.json` via `--gtest_filter` (see
[Agentic-Risk-Test-Matrix.md](Agentic-Risk-Test-Matrix.md)). Invoked automatically at the end of
`Scripts/ci-llm-linux.sh`. Standalone:

```bash
cmake -S . -B build-llm-ci -DRDK_USE_LLM=ON -DBUILD_TESTING=ON
cmake --build build-llm-ci --target Test_LLM_SessionPersist Test_LLM_ToolGateway \
  Test_LLM_Orchestrator Test_LLM_ArgumentValidator -j"$(nproc)"
BUILD_DIR=build-llm-ci ./Scripts/ci-llm-agentic-risk.sh
```

**CI agent scenario matrix (`ci-llm-agent-scenarios.sh`):** after agentic-risk, runs
`Test_LLM_AgentScenarios` deterministic filter (required) and `E2e/*` (lab Ollama; cases
`GTEST_SKIP` when unreachable). See [Agent-Scenario-Matrix.md](Agent-Scenario-Matrix.md).

```bash
BUILD_DIR=build-llm-ci ./Scripts/ci-llm-agent-scenarios.sh
```

---
