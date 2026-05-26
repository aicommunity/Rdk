# LLM Providers

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

## 2. `UOpenAICompatProvider` (фаза 1)

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

---

## 3. `UOllamaNativeProvider` (optional)

- POST `/api/chat` Ollama native API
- Use if OpenAI-compat tool format issues — fallback path

---

## 4. `UEmbeddedLlamaProvider` (фаза 4, обязательно)

`capabilities().supports_streaming = true`. `chatStream` decodes GGUF token-by-token via `ULlamaRuntime::completeStream` (same path as GUI `LLMStreamHandlers`, TD-045).

**Target:** `rdk.llm.embedded` links llama.cpp.

```cpp
class UEmbeddedLlamaProvider : public ILLMProvider {
    bool loadModel(const std::string& gguf_path, int n_ctx, int n_gpu_layers);
    void unloadModel();
    // chat uses same messages/tools — internal tokenization
};
```

**Runtime:**
- Inference thread pool (size 1 MVP)
- `cancel()` sets atomic on `ULlamaRuntime`

**Capabilities:**
- `runs_in_process = true`
- `requires_network = false`

**Stub (фазы 0–3):** `healthCheck` returns false, message «build with RDK_LLM_BUILD_EMBEDDED».

---

## 5. `ULLMProviderFactory`

```cpp
std::unique_ptr<ILLMProvider> ULLMProviderFactory::create(const LLMProviderProfile& p);
```

`LLMProviderProfile` stored in QSettings JSON:

```json
{
  "profile_id": "ollama-local",
  "kind": "OllamaOpenAICompat",
  "base_url": "http://127.0.0.1:11434/v1",
  "model": "qwen2.5:7b",
  "api_key_env": "",
  "prefer_local": true
}
```

---

## 6. Tool calling parity

HTTP и Embedded **должны** принимать одинаковый `std::vector<LLMToolDefinition>`.

Mapping tool results → `LLMMessage::Role::Tool` per OpenAI spec.

**Критично:** каждый assistant `tool_calls` block получает tool messages с matching `tool_call_id` before next user message ([Anthropic troubleshooting](https://docs.anthropic.com/en/docs/agents-and-tools/tool-use/troubleshooting-tool-use)).

---

## 7. Mock provider (tests)

`ULLMMockProvider` in `Rdk/LLM/Core/Providers/ULLMMockProvider.cpp` — only in test targets.

### Streaming (TD-024)

`UOpenAICompatProvider::chatStream` — `stream: true`, SSE `data:` lines, deltas `content` (+ `tool_calls` без live token stream в GUI).  
`ULLMHttpClient::postJsonStream` — curl write callback; abort when `ILLMProvider::cancel()` / `ULLMAgentOrchestrator::cancel()`.

**Ollama remote (TD-029):** env `NMSDK_LLM_OLLAMA_BASE_URL`, `NMSDK_LLM_OLLAMA_MODEL`; Settings override; `/api/tags` list on 404.

**Strict plan JSON (TD-028):** cloud `OpenAICompat` profiles — `response_format.json_schema` on Plan intent; Ollama uses markdown/json block fallback.

---

## 8. Priority switch UI

Combo order:
1. In-process (GGUF) — if `RDK_LLM_EMBEDDED`
2. Ollama local
3. Remote API profiles

Indicator: green = local, blue = cloud.
