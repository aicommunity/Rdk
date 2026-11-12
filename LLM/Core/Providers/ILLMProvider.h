#ifndef RDK_ILLM_PROVIDER_H
#define RDK_ILLM_PROVIDER_H

#include <functional>

#include "../LlmTypes.h"

namespace RDK::LLM {

class ILLMProvider {
public:
    virtual ~ILLMProvider() = default;
    virtual LLMProviderKind kind() const = 0;
    virtual LLMProviderCapabilities capabilities() const = 0;
    virtual LLMCompletionResult chat(const std::vector<LLMMessage>& messages,
                                     const LLMCompletionOptions& opts) = 0;
    virtual void chatStream(const std::vector<LLMMessage>& messages,
                            const LLMCompletionOptions& opts, LLMStreamCallback on_chunk,
                            std::function<void(LLMCompletionResult)> on_done) = 0;
    virtual bool healthCheck(std::string& error_out) = 0;
    virtual void cancel() = 0;
};

} // namespace RDK::LLM

#endif
