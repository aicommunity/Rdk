#ifndef RDK_ILLM_PROVIDER_SETTINGS_SOURCE_H
#define RDK_ILLM_PROVIDER_SETTINGS_SOURCE_H

#include "../LlmTypes.h"

namespace RDK::LLM {

class ILLMProviderSettingsSource {
public:
    virtual ~ILLMProviderSettingsSource() = default;
    virtual LLMRuntimeProviderSettings load() const = 0;
    virtual void save(const LLMRuntimeProviderSettings& /*settings*/) {}
};

} // namespace RDK::LLM

#endif
