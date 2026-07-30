#ifndef RDK_ULLM_SETTINGS_STORE_H
#define RDK_ULLM_SETTINGS_STORE_H

#include "ILLMProviderSettingsSource.h"
#include "ULLMProviderCatalog.h"

namespace RDK::LLM {

class ULLMSettingsStore {
public:
    explicit ULLMSettingsStore(ILLMProviderSettingsSource* source = nullptr);

    void setSource(ILLMProviderSettingsSource* source);
    void reload();
    void save();

    const LLMRuntimeProviderSettings& runtime() const { return m_runtime; }
    LLMProviderProfile activeProfile() const;
    void setActiveProfileId(const std::string& profile_id);
    void setApiKeyForProfile(const std::string& profile_id, const std::string& api_key);
    void setEndpointOverride(const std::string& profile_id, const std::string& base_url,
                             const std::string& model);
    void clearEndpointOverride(const std::string& profile_id);
    LLMProfileEndpointOverride endpointOverride(const std::string& profile_id) const;
    LLMProviderProfile presetProfile(const std::string& profile_id) const;
    void setAllowCloudProviders(bool allow);
    void setLlmWriteEnabled(bool allow);
    void setLlmAutoApplyWrites(bool auto_apply);
    void setAutonomousMode(LLMAutonomousMode mode);
    void setMaxAutonomousSteps(int max_steps);
    void setTranslateQueriesToEn(bool enabled);
    void setTaskPathMode(LLMTaskPathMode mode);
    void setContextAcquisitionMode(LLMContextAcquisitionMode mode);
    void setEnableOllamaThinking(bool enabled);

    void setPreferredResponseLanguage(const std::string& code);
    const std::string& preferredResponseLanguage() const;

    void setSendShortcut(LLMSendShortcutMode mode);
    LLMSendShortcutMode sendShortcut() const;

    std::vector<LLMProviderProfile> listProfiles() const;

private:
    ILLMProviderSettingsSource* m_source = nullptr;
    LLMRuntimeProviderSettings m_runtime;
};

} // namespace RDK::LLM

#endif
