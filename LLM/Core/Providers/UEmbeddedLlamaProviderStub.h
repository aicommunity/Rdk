#ifndef RDK_UEMBEDDED_LLAMA_PROVIDER_STUB_H
#define RDK_UEMBEDDED_LLAMA_PROVIDER_STUB_H

#include "ILLMProvider.h"

namespace RDK::LLM {

class UEmbeddedLlamaProviderStub : public ILLMProvider {
public:
    LLMProviderKind kind() const override { return LLMProviderKind::EmbeddedLlama; }
    LLMProviderCapabilities capabilities() const override;
    LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                             const LLMCompletionOptions& opts) override;
    void chatStream(const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
                    LLMStreamCallback on_chunk,
                    std::function<void(LLMCompletionResult)> on_done) override;
    bool healthCheck(std::string& error_out) override;
    void cancel() override {}
};

} // namespace RDK::LLM

#endif
