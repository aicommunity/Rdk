#include "UOllamaModelInfo.h"

#include "../Http/ULLMHttpClient.h"
#include "UOllamaChatTemplate.h"

#include <map>
#include <mutex>

namespace RDK::LLM {

namespace {

std::mutex g_cache_mutex;
std::map<std::string, OllamaChatTemplateFamily> g_template_cache;

std::string ollamaHostKey(const LLMProviderProfile& profile)
{
    std::string host = profile.base_url;
    while(!host.empty() && host.back() == '/')
        host.pop_back();
    const std::string suffix = "/v1";
    if(host.size() >= suffix.size() && host.compare(host.size() - suffix.size(), suffix.size(), suffix) == 0)
        host.erase(host.size() - suffix.size());
    return host;
}

std::string cacheKey(const LLMProviderProfile& profile)
{
    return ollamaHostKey(profile) + "|" + profile.model;
}

} // namespace

OllamaChatTemplateFamily inferTemplateFamilyFromModelfile(const std::string& modelfile)
{
    const std::string lower = [&]() {
        std::string s = modelfile;
        for(char& c : s)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }();

    if(lower.find("<|im_start|>") != std::string::npos || lower.find("im_end") != std::string::npos)
        return OllamaChatTemplateFamily::Qwen2;
    if(lower.find("start_header_id") != std::string::npos || lower.find("begin_of_text") != std::string::npos)
        return OllamaChatTemplateFamily::Llama3;
    if(lower.find("[inst]") != std::string::npos)
        return OllamaChatTemplateFamily::Mistral;
    if(lower.find("<start_of_turn>") != std::string::npos)
        return OllamaChatTemplateFamily::Gemma2;
    return OllamaChatTemplateFamily::ChatML;
}

OllamaChatTemplateFamily fetchOllamaTemplateFamily(const LLMProviderProfile& profile)
{
    if(profile.model.empty())
        return OllamaChatTemplateFamily::Auto;

    const std::string key = cacheKey(profile);
    {
        std::lock_guard<std::mutex> lock(g_cache_mutex);
        if(auto it = g_template_cache.find(key); it != g_template_cache.end())
            return it->second;
    }

    OllamaChatTemplateFamily resolved = OllamaChatTemplateFamily::Auto;
    if(isOllamaProvider(profile))
    {
        ULLMHttpClient http;
        const std::string url = ollamaHostKey(profile) + "/api/show";
        const nlohmann::json body = {{"model", profile.model}};
        const auto resp = http.postJson(url, body.dump(), profile.api_key, 8000);
        if(resp.status_code >= 200 && resp.status_code < 300 && !resp.body.empty())
        {
            try
            {
                const nlohmann::json j = nlohmann::json::parse(resp.body);
                std::string modelfile = j.value("modelfile", "");
                if(modelfile.empty() && j.contains("template"))
                    modelfile = j["template"].get<std::string>();
                if(!modelfile.empty())
                    resolved = inferTemplateFamilyFromModelfile(modelfile);
            }
            catch(...)
            {
                resolved = OllamaChatTemplateFamily::Auto;
            }
        }
    }

    if(resolved == OllamaChatTemplateFamily::Auto)
        resolved = detectChatTemplateFamily(profile.model, profile.chat_template);

    std::lock_guard<std::mutex> lock(g_cache_mutex);
    g_template_cache[key] = resolved;
    return resolved;
}

} // namespace RDK::LLM
