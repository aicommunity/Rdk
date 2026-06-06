# Testing Strategy

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

## 2. Mock `ILLMProvider`

```cpp
class ULLMMockProvider : public ILLMProvider {
    // scripted responses: tool_calls sequence or text
    void enqueueResponse(LLMCompletionResult r);
};
```

Fixtures: `Rdk/Tests/Fixtures/LLM/provider/*.json`

---

## 3. Regression set

`Rdk/Tests/Fixtures/LLM/regression/`:

| File | User phrase (RU) | Expected first tool |
|------|------------------|---------------------|
| `q_list_components.txt` | какие компоненты на схеме | `get_net_snapshot` |
| `q_add_source.txt` | добавь MatrixSource | `find_component` or `add_component` |
| `q_class_help.txt` | что такое ArduinoBoard | `describe_class` |

Runner: `Test_LLM_RegressionFixtures` — все `regression/*.json` → `ULLMIntentParser` (без HTTP).  
Legacy: `q_list_components.txt` + `Test_LLM_RegressionIntent` (registry smoke).

---

## 4. Policy tests

```cpp
TEST(LLMPolicy, DenyWriteWithoutProject) {
  LLMSessionContext s; s.project_loaded = false;
  auto d = policy.checkToolInvoke(req_add_component, ...);
  EXPECT_FALSE(d.allowed);
  EXPECT_EQ(d.deny_code, "PROJECT_NOT_LOADED");
}
```

---

## 5. Tool selection tests

Intent parser only — no network.

---

## 6. Argument extraction

Golden JSON: model output (from fixtures) → validator → expected pass/fail.

---

## 7. Audit tests

- Append 3 events → verify hash chain
- Tamper middle line → verify fails

---

## 8. CMake

```cmake
if(RDK_USE_LLM AND BUILD_TESTS)
  add_subdirectory(Rdk/Tests/Unit/LLM)
  add_subdirectory(Rdk/Tests/Integration/LLM)
endif()
```

---

## 9. GUI smoke (manual)

1. Open NeuroModeler with LLM enabled
2. Open AI dock, send «список компонентов»
3. Select component on diagram, ask «свойства этого компонента»
4. (фаза 2) Add with preview cancel
5. Provider switch Ollama ↔ disabled cloud
