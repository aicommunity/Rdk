#include "ULLMSettingsStore.h"

#include "ULLMProviderAuth.h"

namespace RDK::LLM {

ULLMSettingsStore::ULLMSettingsStore(ILLMProviderSettingsSource* source)
    : m_source(source)
{
    reload();
}

void ULLMSettingsStore::setSource(ILLMProviderSettingsSource* source)
{
    m_source = source;
    reload();
}

void ULLMSettingsStore::reload()
{
    if(m_source)
        m_runtime = m_source->load();
    else
        m_runtime = LLMRuntimeProviderSettings{};
}

void ULLMSettingsStore::save()
{
    if(m_source)
        m_source->save(m_runtime);
}

LLMProviderProfile ULLMSettingsStore::activeProfile() const
{
    LLMProviderProfile profile = ULLMProviderCatalog::resolveActive(m_runtime);
    profile.api_key = ULLMProviderAuth::resolveApiKey(profile, m_runtime);
    return profile;
}

void ULLMSettingsStore::setActiveProfileId(const std::string& profile_id)
{
    m_runtime.active_profile_id = profile_id;
}

void ULLMSettingsStore::setApiKeyForProfile(const std::string& profile_id,
                                             const std::string& api_key)
{
    m_runtime.api_keys_by_profile_id[profile_id] = api_key;
}

void ULLMSettingsStore::setEndpointOverride(const std::string& profile_id,
                                            const std::string& base_url,
                                            const std::string& model)
{
    m_runtime.endpoint_overrides_by_profile_id[profile_id] = {base_url, model};
}

void ULLMSettingsStore::clearEndpointOverride(const std::string& profile_id)
{
    m_runtime.endpoint_overrides_by_profile_id.erase(profile_id);
}

LLMProfileEndpointOverride ULLMSettingsStore::endpointOverride(
    const std::string& profile_id) const
{
    if(auto it = m_runtime.endpoint_overrides_by_profile_id.find(profile_id);
       it != m_runtime.endpoint_overrides_by_profile_id.end())
        return it->second;
    return {};
}

LLMProviderProfile ULLMSettingsStore::presetProfile(const std::string& profile_id) const
{
    if(const LLMProviderProfile* preset = ULLMProviderCatalog::findById(profile_id))
        return *preset;
    return ULLMProviderCatalog::builtInProfiles().front();
}

void ULLMSettingsStore::setAllowCloudProviders(bool allow)
{
    m_runtime.allow_cloud_providers = allow;
}

void ULLMSettingsStore::setLlmWriteEnabled(bool allow)
{
    m_runtime.llm_write_enabled = allow;
}

void ULLMSettingsStore::setLlmAutoApplyWrites(bool auto_apply)
{
    m_runtime.llm_auto_apply_writes = auto_apply;
}

void ULLMSettingsStore::setAutonomousMode(LLMAutonomousMode mode)
{
    m_runtime.autonomous_mode = mode;
}

void ULLMSettingsStore::setMaxAutonomousSteps(int max_steps)
{
    m_runtime.max_autonomous_steps = max_steps > 0 ? max_steps : 3;
}

void ULLMSettingsStore::setTranslateQueriesToEn(bool enabled)
{
    m_runtime.translate_queries_to_en = enabled;
}

void ULLMSettingsStore::setTaskPathMode(const LLMTaskPathMode mode)
{
    m_runtime.task_path_mode = mode;
}

void ULLMSettingsStore::setContextAcquisitionMode(const LLMContextAcquisitionMode mode)
{
    m_runtime.context_acquisition_mode = mode;
}

void ULLMSettingsStore::setEnableOllamaThinking(bool enabled)
{
    m_runtime.enable_ollama_thinking = enabled;
}

void ULLMSettingsStore::setPreferredResponseLanguage(const std::string& code)
{
    m_runtime.preferred_response_language = code;
}

const std::string& ULLMSettingsStore::preferredResponseLanguage() const
{
    return m_runtime.preferred_response_language;
}

void ULLMSettingsStore::setSendShortcut(LLMSendShortcutMode mode)
{
    m_runtime.send_shortcut = mode;
}

LLMSendShortcutMode ULLMSettingsStore::sendShortcut() const
{
    return m_runtime.send_shortcut;
}

std::vector<LLMProviderProfile> ULLMSettingsStore::listProfiles() const
{
    return ULLMProviderCatalog::builtInProfiles();
}

} // namespace RDK::LLM
