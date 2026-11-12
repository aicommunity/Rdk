# Tool Gateway and Registry

## RU

## 1. Tool-first (не prompt-first)

Все действия над доменом — **именованные tools** с JSON Schema. Промпт объясняет *когда* вызывать tool, но не *как* обойти схему.

Практика (production agent gateways): registry публикует schema до вызова; gateway валидирует input **до** domain; версии tools для regression ([schema-gateway](https://github.com/sravan27/schema-gateway), micro-agents pattern).

---

## EN

## 2. `ULLMToolRegistry`

### 2.1 Entry tool

```cpp
struct LLMToolDefinition {
    std::string name;              // snake_case, stable
    std::string version;           // semver, e.g. "1.0.0"
    LLMToolKind kind;              // Read | Write
    std::string description;       // для LLM API
    nlohmann::json input_schema;   // JSON Schema draft 2020-12 subset
    nlohmann::json output_schema;
    bool strict = true;            // OpenAI strict mode where supported
    bool requires_confirmation = false;
    bool idempotent = false;
    std::vector<std::string> required_policy_tags; // e.g. "project_loaded"
};

class ULLMToolRegistry {
public:
    static constexpr const char* REGISTRY_VERSION = "1.0.0";

    void registerTool(const LLMToolDefinition& def,
                      std::function<ToolResult(const nlohmann::json&)> handler);
    std::vector<LLMToolDefinition> listForLlmApi(const ToolFilter& filter) const;
    const LLMToolDefinition* find(const std::string& name) const;
};
```

### 2.2 Versioning

| Constant | When change |
|-----------|--------------|
| `TOOL_REGISTRY_VERSION` | Any breaking change input_schema write-tool |
| `tool.version` per tool | Minor: addition optional fields |

Audit log **must** record `registry_version` + `tool.version`.

### 2.3 Filter tools в prompt

**Never** не pass все tools в one request.

```cpp
struct ToolFilter {
    LLMIntentKind intent;           // Query, Mutate, Explain
    bool include_write = false;
    std::string focus_class_name;   // сузить describe_* 
};
```

Example:
- Intent `Query` → only Read tools (6 шт.)
- Intent `Mutate` → Read + Write (subset)

---
