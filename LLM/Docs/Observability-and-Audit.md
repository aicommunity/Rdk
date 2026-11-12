# Observability and Audit

## RU

## 1. Требование

Без audit/trace подсистема **не поддерживается** в production.

Практики: append-only JSONL, hash chain (`prev_hash`, `curr_hash`), correlation id ([audit trails LLM](https://arxiv.org/html/2601.20727v1), [Microsoft agent-governance-toolkit](https://github.com/microsoft/agent-governance-toolkit)).

---

## EN

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

Passed во все layers на one user message.

---
