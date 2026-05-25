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

void ULLMSettingsStore::setAllowCloudProviders(bool allow)
{
    m_runtime.allow_cloud_providers = allow;
}

void ULLMSettingsStore::setLlmWriteEnabled(bool allow)
{
    m_runtime.llm_write_enabled = allow;
}

std::vector<LLMProviderProfile> ULLMSettingsStore::listProfiles() const
{
    return ULLMProviderCatalog::builtInProfiles();
}

} // namespace RDK::LLM
