# Tool Gateway and Registry

## 1. Tool-first (не prompt-first)

Все действия над доменом — **именованные tools** с JSON Schema. Промпт объясняет *когда* вызывать tool, но не *как* обойти схему.

Практика (production agent gateways): registry публикует schema до вызова; gateway валидирует input **до** domain; версии tools для regression ([schema-gateway](https://github.com/sravan27/schema-gateway), micro-agents pattern).

---

## 2. `ULLMToolRegistry`

### 2.1 Запись tool

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

### 2.2 Версионирование

| Константа | Когда менять |
|-----------|--------------|
| `TOOL_REGISTRY_VERSION` | Любое breaking изменение input_schema write-tool |
| `tool.version` per tool | Minor: добавление optional полей |

Audit log **обязан** записывать `registry_version` + `tool.version`.

### 2.3 Фильтр tools в prompt

**Никогда** не передавать все tools в одном запросе.

```cpp
struct ToolFilter {
    LLMIntentKind intent;           // Query, Mutate, Explain
    bool include_write = false;
    std::string focus_class_name;   // сузить describe_* 
};
```

Пример:
- Intent `Query` → только Read tools (6 шт.)
- Intent `Mutate` → Read + Write (подмножество)

---

## 3. `ULLMToolGateway` — единственная точка исполнения

```cpp
class ULLMToolGateway {
public:
    ULLMToolGateway(ULLMToolRegistry& registry,
                    ULLMPolicyEngine& policy,
                    URdkDomainAccess& domain,
                    ULLMAuditLog& audit,
                    ULLMIdempotencyStore& idempotency);

    ToolGatewayResult invoke(const ToolInvokeRequest& req);
    ToolGatewayResult invokeBatch(const std::vector<ToolInvokeRequest>& reqs,
                                 bool parallel_read);  // фаза 3
};
```

### 3.1 `ToolInvokeRequest`

```cpp
struct ToolInvokeRequest {
    std::string trace_id;
    std::string tool_name;
    nlohmann::json arguments;
    std::string idempotency_key;  // обязателен для write + idempotent tools
    LLMSessionContext session;    // user, channel, roles
};
```

### 3.2 Порядок обработки (строго)

```
1. find tool in registry → NotFound
2. ULLMToolArgumentValidator::validate(arguments, input_schema)
3. ULLMPolicyEngine::checkToolInvoke(request, tool_def)
4. if Write && requires_confirmation → return PendingConfirmation (не вызывать handler)
5. if idempotent → idempotency store lookup → return cached if hit
6. audit: tool_invoke_started
7. handler (→ URdkDomainAccess)
8. audit: tool_invoke_finished
9. if idempotent → store result
10. return ToolGatewayResult
```

### 3.3 `ToolGatewayResult`

```cpp
struct ToolGatewayResult {
    bool ok;
    nlohmann::json result;           // соответствует output_schema
    std::string error_code;
    std::string message;
    bool pending_confirmation = false;
    std::string confirmation_id;
};
```

---

## 4. `ULLMToolArgumentValidator`

- Библиотека: `nlohmann::json` + встроенная валидация required/type/enum **или** отдельный jsonschema validator (фаза 3).
- MVP: проверка required fields, type string/int/number/bool, enum arrays из schema.
- При ошибке — **не** вызывать domain; вернуть модели:

```json
{
  "ok": false,
  "error_code": "SchemaValidationFailed",
  "message": "status must be one of: open, in_progress, closed",
  "validation_errors": [{"path": "/status", "detail": "..."}]
}
```

---

## 5. Read vs Write

| Kind | Policy | Confirmation | Idempotency | Parallel |
|------|--------|--------------|-------------|----------|
| Read | Мягче | Нет | N/A | Да (фаза 3) |
| Write | Строгий | По флагу tool | Рекомендуется | Нет |

---

## 6. Pending confirmation flow

1. Gateway возвращает `pending_confirmation` + `confirmation_id`
2. Orchestrator сохраняет в `ULLMConversationStore::pending`
3. GUI `ULlmChangePreviewWidget` показывает plan
4. User Confirm → `invoke` с тем же arguments + `confirmation_token`
5. Policy проверяет token match + TTL (default 10 min)

---

## 7. Идемпотентность

```cpp
// Ключ: hash(session_id + tool_name + canonical_json(arguments))
class ULLMIdempotencyStore {
    std::optional<ToolGatewayResult> find(const std::string& key);
    void put(const std::string& key, const ToolGatewayResult& result,
             std::chrono::minutes ttl = 15min);
};
```

Write-tools с `idempotent: true` по умолчанию:
- `add_component` (если передан `client_request_id`)
- `set_property`

---

## 8. Регистрация core tools

```cpp
// Rdk/LLM/Core/Tools/RegisterCoreRdkTools.cpp
void RegisterCoreRdkTools(ULLMToolRegistry& reg, URdkDomainAccess& domain, ...);
```

Вызывается из `NmsdkRegisterLlm` **после** создания domain access.

Полные схемы: [Tools-Contracts.md](Tools-Contracts.md).
