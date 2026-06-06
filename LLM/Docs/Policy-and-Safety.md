# Policy and Safety

## RU

## 1. Принцип

**Guardrails вне LLM.** System prompt дополняет, но не заменяет `ULLMPolicyEngine`.

Практика: policy-as-code (YAML/JSON rules или C++ rules engine), audit deny events ([governed-llm-gateway](https://github.com/NathanMaine/governed-llm-gateway), [Microsoft agent-governance-toolkit](https://github.com/microsoft/agent-governance-toolkit)).

---

## EN

## 2. `ULLMPolicyEngine`

```cpp
class ULLMPolicyEngine {
public:
    PolicyDecision checkToolInvoke(const ToolInvokeRequest& req,
                                   const LLMToolDefinition& tool,
                                   const URdkDomainAccess& domain,
                                   const LLMSessionContext& session);

    PolicyDecision checkPlan(const ULLMExecutionPlan& plan,
                             const LLMSessionContext& session);

    PolicyDecision checkRetrievedContext(const RetrievedContext& ctx);
};
```

```cpp
struct PolicyDecision {
    bool allowed;
    std::string deny_code;   // PROJECT_NOT_LOADED, WRITE_DISABLED, RATE_LIMIT, ...
    std::string deny_message;
};
```

---
