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
| L7 E2E | Mock HTTP LLM | Integration | Optional |
| L8 Manual | Real Ollama | Manual checklist | Yes |

**CI default (`RDK_USE_LLM=OFF`):** L1–L6 **не собираются**.

**CI `llm-linux`:** `cmake -DRDK_USE_LLM=ON` + run L1–L7.

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

Runner (фаза 3): `Rdk/Tests/Integration/LLM/test_regression.cpp`

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
