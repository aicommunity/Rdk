#include "ULLMProviderAuth.h"

#include <cstdlib>

namespace RDK::LLM {

std::string ULLMProviderAuth::resolveApiKey(const LLMProviderProfile& profile,
                                            const LLMRuntimeProviderSettings& runtime)
{
    if(!profile.api_key.empty())
        return profile.api_key;
    if(auto it = runtime.api_keys_by_profile_id.find(profile.profile_id);
       it != runtime.api_keys_by_profile_id.end())
        return it->second;
    if(!profile.api_key_env.empty())
    {
        if(const char* env = std::getenv(profile.api_key_env.c_str()))
            return env;
    }
    return {};
}

bool ULLMProviderAuth::hasApiKey(const LLMProviderProfile& profile,
                                 const LLMRuntimeProviderSettings& runtime)
{
    return !resolveApiKey(profile, runtime).empty();
}

ProviderAccessCheck ULLMProviderAuth::checkProviderAccess(
    const LLMProviderProfile& profile, const LLMRuntimeProviderSettings& runtime)
{
    ProviderAccessCheck result;
    if(profile.is_cloud && !runtime.allow_cloud_providers)
    {
        result.allowed = false;
        result.deny_code = "CLOUD_PROVIDER_DISABLED";
        result.deny_message =
            "Cloud LLM providers are disabled. Enable in AI Settings or use Ollama local.";
        return result;
    }
    if(profile.is_cloud && !hasApiKey(profile, runtime))
    {
        result.allowed = false;
        result.deny_code = "API_KEY_MISSING";
        result.deny_message = "API key required for profile: " + profile.profile_id +
                              ". Set in AI Settings or env " + profile.api_key_env + ".";
        return result;
    }
    return result;
}

ProviderAccessCheck ULLMProviderAuth::checkProviderAccess(
    const LLMProviderProfile& profile, const LLMSessionContext& session,
    const LLMRuntimeProviderSettings& runtime)
{
    LLMRuntimeProviderSettings merged = runtime;
    if(!session.allow_cloud_llm)
        merged.allow_cloud_providers = false;
    return checkProviderAccess(profile, merged);
}

} // namespace RDK::LLM
