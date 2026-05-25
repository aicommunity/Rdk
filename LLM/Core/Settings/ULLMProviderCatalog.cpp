#include "ULLMProviderCatalog.h"

namespace RDK::LLM {

std::vector<LLMProviderProfile> ULLMProviderCatalog::builtInProfiles()
{
    std::vector<LLMProviderProfile> profiles;

    LLMProviderProfile ollama;
    ollama.profile_id = "ollama-local";
    ollama.kind = LLMProviderKind::OllamaOpenAICompat;
    ollama.base_url = "http://127.0.0.1:11434/v1";
    ollama.model = "qwen2.5:7b";
    ollama.is_cloud = false;
    ollama.prefer_local = true;
    profiles.push_back(ollama);

    LLMProviderProfile deepseek;
    deepseek.profile_id = "deepseek";
    deepseek.kind = LLMProviderKind::OpenAICompat;
    deepseek.base_url = "https://api.deepseek.com/v1";
    deepseek.model = "deepseek-chat";
    deepseek.api_key_env = "DEEPSEEK_API_KEY";
    deepseek.is_cloud = true;
    profiles.push_back(deepseek);

    LLMProviderProfile openai;
    openai.profile_id = "openai";
    openai.kind = LLMProviderKind::OpenAICompat;
    openai.base_url = "https://api.openai.com/v1";
    openai.model = "gpt-4o-mini";
    openai.api_key_env = "OPENAI_API_KEY";
    openai.is_cloud = true;
    profiles.push_back(openai);

    return profiles;
}

const LLMProviderProfile* ULLMProviderCatalog::findById(const std::string& profile_id)
{
    static const std::vector<LLMProviderProfile> kProfiles = builtInProfiles();
    for(const LLMProviderProfile& p : kProfiles)
    {
        if(p.profile_id == profile_id)
            return &p;
    }
    return nullptr;
}

LLMProviderProfile ULLMProviderCatalog::resolveActive(const LLMRuntimeProviderSettings& runtime)
{
    if(const LLMProviderProfile* preset = findById(runtime.active_profile_id))
        return *preset;
    return builtInProfiles().front();
}

} // namespace RDK::LLM
