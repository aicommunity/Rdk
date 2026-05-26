#include "UEmbeddedLlamaProvider.h"

#include <cstdlib>

namespace RDK::LLM {

namespace {

LlamaRuntimeConfig runtimeConfigFromProfile(const LLMProviderProfile& profile)
{
    LlamaRuntimeConfig cfg;
    cfg.gguf_path = resolveGgufPath(profile);
    if(const char* ctx_env = std::getenv("NMSDK_LLM_CTX"))
        cfg.n_ctx = std::max(512, std::atoi(ctx_env));
    if(const char* gpu_env = std::getenv("NMSDK_LLM_GPU_LAYERS"))
        cfg.n_gpu_layers = std::atoi(gpu_env);
    if(const char* threads_env = std::getenv("NMSDK_LLM_THREADS"))
        cfg.n_threads = std::max(1, std::atoi(threads_env));
    return cfg;
}

} // namespace

UEmbeddedLlamaProvider::UEmbeddedLlamaProvider(LLMProviderProfile profile)
    : m_profile(std::move(profile))
{
}

LLMProviderCapabilities UEmbeddedLlamaProvider::capabilities() const
{
    LLMProviderCapabilities c;
    c.supports_tool_calling = false;
    c.supports_streaming = true;
    c.runs_in_process = true;
    c.requires_network = false;
    return c;
}

bool UEmbeddedLlamaProvider::ensureLoaded(std::string& error_out)
{
    if(m_runtime.isLoaded())
        return true;
    return m_runtime.loadModel(runtimeConfigFromProfile(m_profile), error_out);
}

LLMCompletionResult UEmbeddedLlamaProvider::chat(const std::vector<LLMMessage>& messages,
                                               const LLMCompletionOptions& opts)
{
    LLMCompletionResult result;
    std::string err;
    if(!ensureLoaded(err))
    {
        result.ok = false;
        result.error_message = err;
        return result;
    }
    return m_runtime.complete(messages, opts);
}

void UEmbeddedLlamaProvider::chatStream(const std::vector<LLMMessage>& messages,
                                        const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                                        std::function<void(LLMCompletionResult)> on_done)
{
    std::string err;
    if(!ensureLoaded(err))
    {
        LLMCompletionResult r;
        r.ok = false;
        r.error_message = err;
        if(on_done)
            on_done(r);
        return;
    }

    std::function<void(const std::string&)> chunk_fn;
    if(on_chunk)
        chunk_fn = on_chunk;

    LLMCompletionResult r = m_runtime.completeStream(messages, opts, chunk_fn);
    if(on_done)
        on_done(r);
}

bool UEmbeddedLlamaProvider::healthCheck(std::string& error_out)
{
    const std::string path = resolveGgufPath(m_profile);
    if(path.empty())
    {
        error_out =
            "Set GGUF path in AI Assistant Settings (Model field) or NMSDK_LLM_GGUF_PATH";
        return false;
    }
    LLMProviderProfile with_path = m_profile;
    with_path.model = path;
    m_profile.model = path;
    return m_runtime.loadModel(runtimeConfigFromProfile(with_path), error_out);
}

void UEmbeddedLlamaProvider::cancel()
{
    m_runtime.cancel();
}

} // namespace RDK::LLM
