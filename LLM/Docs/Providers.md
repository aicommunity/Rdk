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
};

struct LLMCompletionOptions {
    std::vector<LLMToolDefinition> tools;
    int max_tokens = 4096;
    float temperature = 0.2f;
    bool stream = false;
    std::optional<nlohmann::json> response_format;  // json_schema for plan
};

class ILLMProvider {
public:
    virtual ~ILLMProvider() = default;
    virtual LLMProviderKind kind() const = 0;
    virtual LLMProviderCapabilities capabilities() const = 0;
    virtual LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                                     const LLMCompletionOptions& opts) = 0;
    virtual void chatStream(const std::vector<LLMMessage>& messages,
                            const LLMCompletionOptions& opts,
                            LLMStreamCallback on_chunk,
                            std::function<void(LLMCompletionResult)> on_done) = 0;
    virtual bool healthCheck(std::string& error_out) = 0;
    virtual void cancel() = 0;
};

} // namespace
```

---

## EN

## 2. `UOpenAICompatProvider` (phase 1)

**HTTP:** `ULLMHttpClient` (libcurl).

**Endpoints:**
- POST `{base_url}/chat/completions`
- Body: OpenAI chat format with `tools`, `tool_choice: auto`

**Profiles (presets):**

| Profile ID | base_url | model default |
|------------|----------|---------------|
| `ollama-local` | `http://127.0.0.1:11434/v1` | `qwen2.5:7b` |
| `deepseek` | `https://api.deepseek.com/v1` | `deepseek-chat` |
| `openai` | `https://api.openai.com/v1` | `gpt-4o-mini` |

**Auth:** header `Authorization: Bearer <api_key>` if non-empty.

**healthCheck:** GET or minimal `chat` with `max_tokens: 1`.

**Retries:** up to 3 attempts on transport errors and HTTP `408` / `429` / `5xx`. Delay is
`max(exponential_backoff, Retry-After)` capped at 60s (`ULLMHttpRetry`, `ULLMHttpClient::Response::retry_after`).

---
