#ifndef RDK_ULLM_MOCK_PROVIDER_H
#define RDK_ULLM_MOCK_PROVIDER_H

#include "ILLMProvider.h"
#include <deque>

namespace RDK::LLM {

class ULLMMockProvider : public ILLMProvider {
public:
    void enqueue(LLMCompletionResult r) { m_queue.push_back(std::move(r)); }

    LLMProviderKind kind() const override { return LLMProviderKind::Mock; }
    LLMProviderCapabilities capabilities() const override;
    LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                             const LLMCompletionOptions& opts) override;
    void chatStream(const std::vector<LLMMessage>& messages, const LLMCompletionOptions& opts,
                    LLMStreamCallback on_chunk,
                    std::function<void(LLMCompletionResult)> on_done) override;
    bool healthCheck(std::string& error_out) override;
    void cancel() override {}

private:
    std::deque<LLMCompletionResult> m_queue;
};

} // namespace RDK::LLM

#endif
