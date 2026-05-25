#ifndef RDK_UOPENAI_COMPAT_PROVIDER_H
#define RDK_UOPENAI_COMPAT_PROVIDER_H

#include "ILLMProvider.h"
#include "../Http/ULLMHttpClient.h"

namespace RDK::LLM {

class UOpenAICompatProvider : public ILLMProvider {
public:
    explicit UOpenAICompatProvider(LLMProviderProfile profile);

    LLMProviderKind kind() const override { return m_profile.kind; }
    LLMProviderCapabilities capabilities() const override;
    LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                             const LLMCompletionOptions& opts) override;
    void chatStream(const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
                    LLMStreamCallback on_chunk,
                    std::function<void(LLMCompletionResult)> on_done) override;
    bool healthCheck(std::string& error_out) override;
    void cancel() override;

private:
    LLMProviderProfile m_profile;
    ULLMHttpClient m_http;
    nlohmann::json buildRequestBody(const std::vector<LLMMessage>& messages,
                                    const LLMCompletionOptions& opts) const;
    LLMCompletionResult parseResponse(const std::string& body, std::string& error) const;

    std::atomic<bool> m_cancelled{false};
};

} // namespace RDK::LLM

#endif
