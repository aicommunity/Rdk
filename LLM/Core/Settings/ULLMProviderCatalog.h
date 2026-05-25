#ifndef RDK_ULLM_PROVIDER_CATALOG_H
#define RDK_ULLM_PROVIDER_CATALOG_H

#include "../LlmTypes.h"
#include <vector>

namespace RDK::LLM {

class ULLMProviderCatalog {
public:
    static std::vector<LLMProviderProfile> builtInProfiles();
    static const LLMProviderProfile* findById(const std::string& profile_id);
    static LLMProviderProfile resolveActive(const LLMRuntimeProviderSettings& runtime);
};

} // namespace RDK::LLM

#endif
