#include "UEmbeddedLlamaProviderStub.h"

namespace RDK::LLM {

LLMProviderCapabilities UEmbeddedLlamaProviderStub::capabilities() const
{
    LLMProviderCapabilities c;
    c.runs_in_process = true;
    c.requires_network = false;
#ifdef RDK_LLM_EMBEDDED
    c.supports_tool_calling = true;
#else
    c.supports_tool_calling = false;
#endif
    return c;
}

LLMCompletionResult UEmbeddedLlamaProviderStub::chat(const std::vector<LLMMessage>& messages,
                                                     const LLMCompletionOptions& opts)
{
    (void)messages;
    (void)opts;
    LLMCompletionResult r;
    r.ok = false;
#ifndef RDK_LLM_EMBEDDED
    r.error_message =
        "Embedded offline model unavailable. Enable RDK_LLM_BUILD_EMBEDDED or use ollama-local.";
#else
    r.error_message =
        "Embedded GGUF runtime not linked yet. Set model path in profile when implemented.";
#endif
    return r;
}

void UEmbeddedLlamaProviderStub::chatStream(const std::vector<LLMMessage>& messages,
                                            const LLMCompletionOptions& opts,
                                            LLMStreamCallback on_chunk,
                                            std::function<void(LLMCompletionResult)> on_done)
{
    LLMCompletionResult r = chat(messages, opts);
    if(on_done)
        on_done(r);
    (void)on_chunk;
}

bool UEmbeddedLlamaProviderStub::healthCheck(std::string& error_out)
{
#ifndef RDK_LLM_EMBEDDED
    error_out = "Build with RDK_LLM_BUILD_EMBEDDED=ON";
    return false;
#else
    error_out = "Not implemented";
    return false;
#endif
}

} // namespace RDK::LLM
