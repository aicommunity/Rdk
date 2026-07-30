# LLM Providers

## RU

## 1. `ILLMProvider`

```cpp
namespace RDK::LLM {

enum class LLMProviderKind {
    OllamaOpenAICompat,   // http://127.0.0.1:11434/v1
    OpenAICompat,         // DeepSeek, OpenAI, custom base_url
    OllamaNative,         // /api/chat (optional)
    EmbeddedLlama,        // in-process, phase 4
    Custom
};

struct LLMProviderCapabilities {
    bool supports_tool_calling = true;
    bool supports_streaming = true;
    bool supports_strict_json_schema = false;  // set per provider
    bool runs_in_process = false;
    bool requires_network = true;
    bool supports_thinking = false;  // Ollama think / message.thinking
};

struct LLMCompletionOptions {
    std::vector<LLMToolDefinition> tools;
    int max_tokens = 4096;
    float temperature = 0.2f;
    bool stream = false;
    std::optional<nlohmann::json> response_format;  // json_schema for plan
    LLMThinkMode think_mode = LLMThinkMode::Off;    // Ollama `think`
};
```

**Thinking-first (Ollama):** Cortex enables `think: true` when `enable_ollama_thinking` and the provider sets `supports_thinking`. Prefer profile `ollama-thinking` (native `/api/chat`, model `qwen3:14b` / `NMSDK_LLM_OLLAMA_THINKING_MODEL`). Reasoning is returned in `message.thinking` (and preserved on assistant tool-call turns). Never force `tool_choice` while thinking is on. GUI shows a collapsible Reasoning block (not the answer bubble).

---

## EN

## 2. `UOpenAICompatProvider` (phase 1)

**HTTP:** `ULLMHttpClient` (libcurl).

**Endpoints:**
- POST `{base_url}/chat/completions`
- Body: OpenAI chat format with `tools`, `tool_choice: auto`, optional `think: true` for Ollama

**Profiles (presets):**

| Profile ID | base_url | model default |
|------------|----------|---------------|
| `ollama-local` | `http://127.0.0.1:11434/v1` | `qwen2.5:14b` |
| `ollama-native` | `http://127.0.0.1:11434` | `qwen2.5:14b` |
| `ollama-thinking` | `http://127.0.0.1:11434` | `qwen3:14b` (env override) |
| `ollama-lite` | `http://127.0.0.1:11434/v1` | `qwen2.5:7b` |
| `deepseek` | `https://api.deepseek.com/v1` | `deepseek-chat` |
| `openai` | `https://api.openai.com/v1` | `gpt-4o-mini` |

**Auth:** header `Authorization: Bearer <api_key>` if non-empty.

**healthCheck:** GET or minimal `chat` with `max_tokens: 1`.

**Retries:** up to 3 attempts on transport errors and HTTP `408` / `429` / `5xx`. Delay is
`max(exponential_backoff, Retry-After)` capped at 60s (`ULLMHttpRetry`, `ULLMHttpClient::Response::retry_after`).

**Settings:** `LLM/enable_ollama_thinking` (default true), `LLM/active_profile_id` default `ollama-thinking`.

---
