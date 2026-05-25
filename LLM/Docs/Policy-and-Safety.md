# Policy and Safety

## 1. Принцип

**Guardrails вне LLM.** System prompt дополняет, но не заменяет `ULLMPolicyEngine`.

Практика: policy-as-code (YAML/JSON rules или C++ rules engine), audit deny events ([governed-llm-gateway](https://github.com/NathanMaine/governed-llm-gateway), [Microsoft agent-governance-toolkit](https://github.com/microsoft/agent-governance-toolkit)).

---

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

## 3. Правила MVP (нормативная таблица)

| ID | Условие | Действие |
|----|---------|----------|
| P01 | `!session.project_loaded` | Deny все Write |
| P02 | `tool.kind == Write` && `!session.llm_write_enabled` (QSettings) | Deny |
| P03 | `tool.name == save_project` && `!session.allow_save` | Deny |
| P04 | `load_project` path outside allowed roots | Deny |
| P05 | `tool_calls_in_turn > max_tool_calls_per_turn` (default 10) | Deny further |
| P06 | `wall_clock_elapsed > max_wall_clock_ms` (default 120000) | Abort turn |
| P07 | `add_component` class_name not in storage | Deny |
| P08 | `long_name` not resolved (entity) | Deny write |
| P09 | API key/cloud provider && `!session.allow_cloud_llm` | Deny provider call |

**Роли пользователя (post-MVP):** map `UApplication::GetUserId()` → RBAC table.

---

## 4. Human-in-the-loop

| Tool | requires_confirmation |
|------|----------------------|
| `add_component` | true |
| `remove_component` | true |
| `set_property` | true (whitelist exceptions: comment-only props — фаза 3) |
| `connect_components` | true |
| `load_project` | true |
| `save_project` | true |
| All Read | false |

GUI: preview plan в `ULlmChangePreviewWidget`; одиночный write — **Apply** → `confirmPending()`.

---

## 5. Лимиты (конфиг `llm.policy.json` или QSettings)

```json
{
  "max_tool_calls_per_turn": 10,
  "max_tool_rounds": 8,
  "max_wall_clock_ms": 120000,
  "max_net_snapshot_components": 500,
  "max_retrieved_doc_chars": 8000,
  "allow_cloud_providers": false,
  "allow_write": true
}
```

---

## 6. Секреты

- API keys: `QSettings` group `LLM/Providers/<profile_id>/api_key` или env `DEEPSEEK_API_KEY`.
- **Запрещено:** писать ключи в project XML, audit log, glog.
- Audit: `api_key_present: true/false` только.

---

## 7. PII / логи

- Не логировать полный user prompt в production (опция debug).
- Tool arguments: логировать JSON с truncate matrix values > 200 chars.

---

## 8. Fallback без LLM

| Условие | Поведение UI |
|---------|--------------|
| Provider healthCheck failed | Banner «LLM недоступен», чат disabled |
| Policy deny cloud | Предложить Ollama local |
| Low confidence (post-MVP) | Suggest manual wizard |

Существующие [`UCreateConfigurationWizardWidget`](../../GUI/Qt/UCreateConfigurationWizardWidget.h) остаются primary path.

---

## 9. Сценарий D (autonomous) — запрет MVP

Если в будущем: `max_autonomous_steps <= 3`, sandbox tools whitelist, обязательный approval каждые N шагов — см. [Anti-Patterns.md](Anti-Patterns.md).
