# Observability and Audit

## 1. Требование

Без audit/trace подсистема **не поддерживается** в production.

Практики: append-only JSONL, hash chain (`prev_hash`, `curr_hash`), correlation id ([audit trails LLM](https://arxiv.org/html/2601.20727v1), [Microsoft agent-governance-toolkit](https://github.com/microsoft/agent-governance-toolkit)).

---

## 2. `ULLMTraceContext`

```cpp
struct ULLMTraceContext {
    std::string trace_id;
    std::string session_id;
    std::string turn_id;
    std::string user_id;           // UApplication::GetUserId()
    std::string program_name;
    int channel_index;
};
```

Передаётся во все слои на один user message.

---

## 3. `ULLMAuditLog`

### 3.1 Event types

| event_type | Когда |
|------------|-------|
| `user_message` | Получен текст (hash content, не raw в prod) |
| `provider_request` | Перед HTTP/inference |
| `provider_response` | После; tokens, latency_ms, model_id |
| `tool_invoke_start` | Gateway step 6 |
| `tool_invoke_finish` | ok/error, duration_ms |
| `policy_deny` | PolicyDecision !allowed |
| `confirmation_requested` | pending_confirmation |
| `confirmation_accepted` | user confirm |
| `confirmation_rejected` | user reject |
| `orchestrator_round` | each agent loop round |
| `error` | uncaught / provider fail |

### 3.2 Event record (JSONL line)

```json
{
  "event_id": "uuid",
  "timestamp": "2026-05-25T12:00:00Z",
  "event_type": "tool_invoke_finish",
  "trace_id": "...",
  "session_id": "...",
  "tool_registry_version": "1.0.0",
  "prompt_bundle_id": "rdk-llm-prompts-1.0.0",
  "model_id": "qwen2.5:7b",
  "provider_kind": "OllamaOpenAICompat",
  "details": {
    "tool_name": "get_net_snapshot",
    "tool_version": "1.0.0",
    "ok": true,
    "duration_ms": 42
  },
  "prev_hash": "sha256...",
  "curr_hash": "sha256..."
}
```

`curr_hash = SHA256(prev_hash + canonical_json(event_without_hashes))`

### 3.3 Storage MVP

- File: `<UserConfig>/LLM/audit/audit-YYYY-MM-DD.jsonl`
- Rotation: daily
- Verify tool (фаза 3): `llm_audit_verify` CLI

**Не хранить:** api_key, full prompts (опция `LLM_AUDIT_VERBOSE=1` debug only).

---

## 4. Версионирование

| Artifact | ID constant |
|----------|-------------|
| Tool registry | `TOOL_REGISTRY_VERSION` |
| Prompt templates | `PROMPT_BUNDLE_ID` |
| Docs bundle | `DOCS_BUNDLE_ID` |
| Provider profile schema | `PROVIDER_PROFILE_SCHEMA_VERSION` |

Каждый `provider_request` логирует все четыре.

---

## 5. Метрики (фаза 3, optional)

Counters (можно glog periodic или файл):

- `llm_requests_total`
- `llm_tool_calls_total{tool_name}`
- `llm_policy_denies_total{code}`
- `llm_provider_latency_ms` histogram
- `llm_tokens_total{direction}`

---

## 6. UI

`ULoggerWidget` — строки level INFO:

```
[LLM][trace=7f3a] tool get_net_snapshot ok 42ms
```

---

## 7. Отладка

Env flags:

| Env | Effect |
|-----|--------|
| `RDK_LLM_LOG_HTTP` | curl verbose (dev only) |
| `RDK_LLM_AUDIT_VERBOSE` | full prompts in audit |
| `RDK_LLM_MOCK_PROVIDER` | stub provider без сети |
