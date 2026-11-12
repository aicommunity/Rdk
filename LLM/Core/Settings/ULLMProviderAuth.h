#ifndef RDK_ULLM_PROVIDER_AUTH_H
#define RDK_ULLM_PROVIDER_AUTH_H

#include "../LlmTypes.h"

namespace RDK::LLM {

struct ProviderAccessCheck {
    bool allowed = true;
    std::string deny_code;
    std::string deny_message;
};

class ULLMProviderAuth {
public:
    static std::string resolveApiKey(const LLMProviderProfile& profile,
                                     const LLMRuntimeProviderSettings& runtime);
    static bool hasApiKey(const LLMProviderProfile& profile,
                          const LLMRuntimeProviderSettings& runtime);
    static ProviderAccessCheck checkProviderAccess(const LLMProviderProfile& profile,
                                                   const LLMRuntimeProviderSettings& runtime);
    static ProviderAccessCheck checkProviderAccess(const LLMProviderProfile& profile,
                                                   const LLMSessionContext& session,
                                                   const LLMRuntimeProviderSettings& runtime);
};

} // namespace RDK::LLM

#endif
