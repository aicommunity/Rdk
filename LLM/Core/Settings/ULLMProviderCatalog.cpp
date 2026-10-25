#include "ULLMProviderCatalog.h"

#include "../Providers/UOllamaModelInfo.h"

#include <cstdlib>

namespace RDK::LLM {

std::vector<LLMProviderProfile> ULLMProviderCatalog::builtInProfiles()
{
    std::vector<LLMProviderProfile> profiles;

    LLMProviderProfile ollama;
    ollama.profile_id = "ollama-local";
    ollama.kind = LLMProviderKind::OllamaOpenAICompat;
    ollama.base_url = "http://127.0.0.1:11434/v1";
    ollama.model = "qwen2.5:14b";
    ollama.chat_template = OllamaChatTemplateFamily::Qwen2;
    ollama.is_cloud = false;
    ollama.prefer_local = true;
    profiles.push_back(ollama);

    LLMProviderProfile ollama_native;
    ollama_native.profile_id = "ollama-native";
    ollama_native.kind = LLMProviderKind::OllamaNative;
    ollama_native.base_url = "http://127.0.0.1:11434";
    ollama_native.model = "qwen2.5:14b";
    ollama_native.chat_template = OllamaChatTemplateFamily::Qwen2;
    ollama_native.is_cloud = false;
    ollama_native.prefer_local = true;
    profiles.push_back(ollama_native);

    LLMProviderProfile ollama_lite;
    ollama_lite.profile_id = "ollama-lite";
    ollama_lite.kind = LLMProviderKind::OllamaOpenAICompat;
    ollama_lite.base_url = "http://127.0.0.1:11434/v1";
    ollama_lite.model = "qwen2.5:7b";
    ollama_lite.chat_template = OllamaChatTemplateFamily::Qwen2;
    ollama_lite.is_cloud = false;
    ollama_lite.prefer_local = true;
    profiles.push_back(ollama_lite);

    LLMProviderProfile ollama_thinking;
    ollama_thinking.profile_id = "ollama-thinking";
    ollama_thinking.kind = LLMProviderKind::OllamaNative;
    ollama_thinking.base_url = "http://127.0.0.1:11434";
    {
        const char* thinking_model = std::getenv("NMSDK_LLM_OLLAMA_THINKING_MODEL");
        ollama_thinking.model =
            (thinking_model && thinking_model[0] != '\0') ? thinking_model : "qwen3:14b";
    }
    ollama_thinking.chat_template = OllamaChatTemplateFamily::Auto;
    ollama_thinking.is_cloud = false;
    ollama_thinking.prefer_local = true;
    profiles.push_back(ollama_thinking);

    LLMProviderProfile embedded;
    embedded.profile_id = "embedded-offline";
    embedded.kind = LLMProviderKind::EmbeddedLlama;
    embedded.base_url = "local://embedded";
    embedded.model = "";
    embedded.is_cloud = false;
    embedded.prefer_local = true;
    profiles.push_back(embedded);

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

LLMProviderProfile ULLMProviderCatalog::applyRuntimeOverrides(
    LLMProviderProfile profile, const LLMRuntimeProviderSettings& runtime)
{
    if(auto it = runtime.endpoint_overrides_by_profile_id.find(profile.profile_id);
       it != runtime.endpoint_overrides_by_profile_id.end())
    {
        if(!it->second.base_url.empty())
            profile.base_url = it->second.base_url;
        if(!it->second.model.empty())
            profile.model = it->second.model;
    }
    applyOllamaEnvironmentDefaults(profile, runtime);
    return profile;
}

LLMProviderProfile ULLMProviderCatalog::resolveActive(const LLMRuntimeProviderSettings& runtime)
{
    LLMProviderProfile profile;
    if(const LLMProviderProfile* preset = findById(runtime.active_profile_id))
        profile = *preset;
    else
        profile = builtInProfiles().front();
    return applyRuntimeOverrides(profile, runtime);
}

} // namespace RDK::LLM
