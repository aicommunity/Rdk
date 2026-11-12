#ifndef RDK_UOLLAMA_NATIVE_PROVIDER_H
#define RDK_UOLLAMA_NATIVE_PROVIDER_H

#include "ILLMProvider.h"
#include "../Http/ULLMHttpClient.h"

namespace RDK::LLM {

class UOllamaNativeProvider : public ILLMProvider {
public:
    explicit UOllamaNativeProvider(LLMProviderProfile profile);

    LLMProviderKind kind() const override { return LLMProviderKind::OllamaNative; }
    LLMProviderCapabilities capabilities() const override;
    LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                             const LLMCompletionOptions& opts) override;
    void chatStream(const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
                    LLMStreamCallback on_chunk,
                    std::function<void(LLMCompletionResult)> on_done) override;
    bool healthCheck(std::string& error_out) override;
    void cancel() override;

private:
    std::string ollamaHost() const;
    LLMCompletionResult parseResponse(const std::string& body, std::string& error) const;

    LLMProviderProfile m_profile;
    ULLMHttpClient m_http;
    std::atomic<bool> m_cancelled{false};
};

} // namespace RDK::LLM

#endif
