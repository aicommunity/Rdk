#ifndef RDK_ULLAMA_RUNTIME_H
#define RDK_ULLAMA_RUNTIME_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "../Core/LlmTypes.h"

#include <functional>

namespace RDK::LLM {

struct LlamaRuntimeConfig {
    std::string gguf_path;
    int n_ctx = 4096;
    int n_gpu_layers = 0;
    int n_threads = 4;
};

class ULlamaRuntime {
public:
    ULlamaRuntime();
    ~ULlamaRuntime();

    bool loadModel(const LlamaRuntimeConfig& config, std::string& error_out);
    void unloadModel();
    bool isLoaded() const;

    LLMCompletionResult complete(const std::vector<LLMMessage>& messages,
                                 const LLMCompletionOptions& opts);
    /// Tokenizes prompt once, invokes on_chunk for each decoded piece (GUI live stream, TD-045).
    LLMCompletionResult completeStream(const std::vector<LLMMessage>& messages,
                                       const LLMCompletionOptions& opts,
                                       const std::function<void(const std::string&)>& on_chunk);
    bool healthCheck(std::string& error_out);
    void cancel();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    std::mutex m_mutex;
    std::atomic<bool> m_cancelled{false};
    LlamaRuntimeConfig m_config;
};

std::string formatMessagesForLlama(const std::vector<LLMMessage>& messages);
std::string resolveGgufPath(const LLMProviderProfile& profile);

} // namespace RDK::LLM

#endif
