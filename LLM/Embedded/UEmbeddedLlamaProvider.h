#ifndef RDK_UEMBEDDED_LLAMA_PROVIDER_H
#define RDK_UEMBEDDED_LLAMA_PROVIDER_H

#include "../Core/Providers/ILLMProvider.h"
#include "ULlamaRuntime.h"

namespace RDK::LLM {

class UEmbeddedLlamaProvider : public ILLMProvider {
public:
    explicit UEmbeddedLlamaProvider(LLMProviderProfile profile);

    LLMProviderKind kind() const override { return LLMProviderKind::EmbeddedLlama; }
    LLMProviderCapabilities capabilities() const override;
    LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                             const LLMCompletionOptions& opts) override;
    void chatStream(const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
                    LLMStreamCallback on_chunk,
                    std::function<void(LLMCompletionResult)> on_done) override;
    bool healthCheck(std::string& error_out) override;
    void cancel() override;

private:
    bool ensureLoaded(std::string& error_out);

    LLMProviderProfile m_profile;
    ULlamaRuntime m_runtime;
};

} // namespace RDK::LLM

#endif
