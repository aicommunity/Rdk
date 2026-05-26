#include "ULlamaRuntime.h"

#include "../Core/Providers/UOllamaChatTemplate.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <sstream>

#include "llama.h"

namespace RDK::LLM {

namespace {

void ensureBackendInit()
{
    static bool initialized = false;
    if(!initialized)
    {
        llama_backend_init();
        ggml_backend_load_all();
        initialized = true;
    }
}

int countPromptTokens(const llama_vocab* vocab, const std::string& prompt)
{
    return -llama_tokenize(vocab, prompt.c_str(), static_cast<int32_t>(prompt.size()), nullptr, 0,
                           true, true);
}

} // namespace

struct ULlamaRuntime::Impl {
    llama_model* model = nullptr;
    llama_context* ctx = nullptr;
    llama_sampler* sampler = nullptr;
    const llama_vocab* vocab = nullptr;
};

std::string formatMessagesForLlama(const std::vector<LLMMessage>& messages)
{
    LLMProviderProfile profile;
    profile.model = "qwen2.5";
    const auto prepared = prepareMessagesForOllama(profile, messages);
    const OllamaChatTemplateFamily family = detectChatTemplateFamily(profile.model, profile.chat_template);
    return formatPromptWithTemplate(family, prepared);
}

ULlamaRuntime::ULlamaRuntime() = default;

ULlamaRuntime::~ULlamaRuntime()
{
    unloadModel();
}

bool ULlamaRuntime::loadModel(const LlamaRuntimeConfig& config, std::string& error_out)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    unloadModel();
    m_cancelled = false;

    if(config.gguf_path.empty())
    {
        error_out = "GGUF model path is empty (set in AI Assistant Settings or NMSDK_LLM_GGUF_PATH)";
        return false;
    }
    if(!std::filesystem::exists(config.gguf_path))
    {
        error_out = "GGUF file not found: " + config.gguf_path;
        return false;
    }

    ensureBackendInit();

    m_impl = std::make_unique<Impl>();

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = config.n_gpu_layers;

    m_impl->model = llama_model_load_from_file(config.gguf_path.c_str(), model_params);
    if(!m_impl->model)
    {
        error_out = "llama_model_load_from_file failed";
        m_impl.reset();
        return false;
    }

    m_impl->vocab = llama_model_get_vocab(m_impl->model);

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = static_cast<uint32_t>(std::max(config.n_ctx, 512));
    ctx_params.n_threads = static_cast<uint32_t>(std::max(config.n_threads, 1));
    ctx_params.n_batch = std::min<uint32_t>(ctx_params.n_ctx, 512);
    ctx_params.no_perf = true;

    m_impl->ctx = llama_init_from_model(m_impl->model, ctx_params);
    if(!m_impl->ctx)
    {
        error_out = "llama_init_from_model failed";
        llama_model_free(m_impl->model);
        m_impl.reset();
        return false;
    }

    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = true;
    m_impl->sampler = llama_sampler_chain_init(sparams);
    llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_temp(0.2f));
    llama_sampler_chain_add(m_impl->sampler, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    m_config = config;
    return true;
}

void ULlamaRuntime::unloadModel()
{
    if(!m_impl)
        return;
    if(m_impl->sampler)
        llama_sampler_free(m_impl->sampler);
    if(m_impl->ctx)
        llama_free(m_impl->ctx);
    if(m_impl->model)
        llama_model_free(m_impl->model);
    m_impl.reset();
}

bool ULlamaRuntime::isLoaded() const
{
    return m_impl && m_impl->model && m_impl->ctx;
}

LLMCompletionResult ULlamaRuntime::complete(const std::vector<LLMMessage>& messages,
                                            const LLMCompletionOptions& opts)
{
    return completeStream(messages, opts, nullptr);
}

LLMCompletionResult ULlamaRuntime::completeStream(
    const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
    const std::function<void(const std::string&)>& on_chunk)
{
    LLMCompletionResult result;
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!isLoaded())
    {
        result.ok = false;
        result.error_message = "Embedded model not loaded";
        return result;
    }

    m_cancelled = false;

    LLMProviderProfile profile;
    profile.model = m_config.gguf_path;
    profile.chat_template = OllamaChatTemplateFamily::Auto;
    const std::string lang = opts.response_language.empty() ? "en" : opts.response_language;
    const auto prepared = prepareMessagesForOllama(profile, messages, lang);
    const OllamaChatTemplateFamily family = resolveChatTemplateFamily(profile);
    const std::string prompt = formatPromptWithTemplate(family, prepared);
    const int n_prompt = countPromptTokens(m_impl->vocab, prompt);
    if(n_prompt <= 0)
    {
        result.ok = false;
        result.error_message = "Failed to tokenize prompt";
        return result;
    }

    std::vector<llama_token> prompt_tokens(static_cast<size_t>(n_prompt));
    if(llama_tokenize(m_impl->vocab, prompt.c_str(), static_cast<int32_t>(prompt.size()),
                      prompt_tokens.data(), static_cast<int32_t>(prompt_tokens.size()), true,
                      true)
       < 0)
    {
        result.ok = false;
        result.error_message = "Tokenization failed";
        return result;
    }

    const int max_gen = std::max(opts.max_tokens, 1);
    std::string generated;
    llama_batch batch = llama_batch_get_one(prompt_tokens.data(),
                                            static_cast<int32_t>(prompt_tokens.size()));

    for(int n_pos = 0; n_pos + batch.n_tokens < n_prompt + max_gen && !m_cancelled;)
    {
        if(llama_decode(m_impl->ctx, batch) != 0)
        {
            result.ok = false;
            result.error_message = "llama_decode failed";
            return result;
        }

        n_pos += batch.n_tokens;
        llama_token new_token = llama_sampler_sample(m_impl->sampler, m_impl->ctx, -1);
        if(llama_vocab_is_eog(m_impl->vocab, new_token))
            break;

        char piece[256];
        const int piece_len =
            llama_token_to_piece(m_impl->vocab, new_token, piece, sizeof(piece), 0, true);
        if(piece_len > 0)
        {
            const std::string chunk(piece, static_cast<size_t>(piece_len));
            generated.append(chunk);
            if(on_chunk)
                on_chunk(chunk);
        }

        batch = llama_batch_get_one(&new_token, 1);
    }

    result.ok = !m_cancelled;
    if(m_cancelled)
        result.error_message = "Generation cancelled";
    result.text = generated;
    result.completion_tokens = static_cast<int>(generated.size() / 4);
    return result;
}

bool ULlamaRuntime::healthCheck(std::string& error_out)
{
    if(isLoaded())
        return true;
    if(m_config.gguf_path.empty())
    {
        error_out = "No GGUF path configured";
        return false;
    }
    return loadModel(m_config, error_out);
}

void ULlamaRuntime::cancel()
{
    m_cancelled = true;
}

} // namespace RDK::LLM
