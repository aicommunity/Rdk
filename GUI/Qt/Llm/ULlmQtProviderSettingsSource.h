#ifndef RDK_ULLM_QT_PROVIDER_SETTINGS_SOURCE_H
#define RDK_ULLM_QT_PROVIDER_SETTINGS_SOURCE_H

#include "../../../LLM/Core/Settings/ILLMProviderSettingsSource.h"

class ULlmQtProviderSettingsSource : public RDK::LLM::ILLMProviderSettingsSource {
public:
    RDK::LLM::LLMRuntimeProviderSettings load() const override;
    void save(const RDK::LLM::LLMRuntimeProviderSettings& settings) override;
};

#endif
