#include "UOllamaModelInfo.h"

#include "../Http/ULLMHttpClient.h"
#include "UOllamaChatTemplate.h"

#include <cstdlib>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>

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

int probeOllamaNumCtx(const LLMProviderProfile& profile)
{
    if(!isOllamaProvider(profile) || profile.model.empty())
        return 0;

    ULLMHttpClient http;
    const std::string url = ollamaHostKey(profile) + "/api/show";
    const nlohmann::json body = {{"model", profile.model}};
    const auto resp = http.postJson(url, body.dump(), profile.api_key, 8000);
    if(resp.status_code < 200 || resp.status_code >= 300 || resp.body.empty())
        return 0;

    try
    {
        const nlohmann::json j = nlohmann::json::parse(resp.body);
        if(j.contains("model_info") && j["model_info"].is_object())
        {
            const auto& info = j["model_info"];
            if(info.contains("context_length") && info["context_length"].is_number_integer())
                return info["context_length"].get<int>();
        }
        const std::string parameters = j.value("parameters", std::string());
        if(!parameters.empty())
        {
            static const std::regex kNumCtx(R"(num_ctx\s+(\d+))");
            std::smatch match;
            if(std::regex_search(parameters, match, kNumCtx) && match.size() > 1)
                return std::stoi(match[1].str());
        }
    }
    catch(...)
    {
    }
    return 0;
}

std::vector<std::string> listOllamaTagModels(const LLMProviderProfile& profile)
{
    std::vector<std::string> names;
    if(profile.kind != LLMProviderKind::OllamaOpenAICompat
       && profile.kind != LLMProviderKind::OllamaNative)
        return names;

    ULLMHttpClient http;
    const auto resp = http.get(ollamaHostKey(profile) + "/api/tags", profile.api_key, 8000);
    if(resp.status_code < 200 || resp.status_code >= 300 || resp.body.empty())
        return names;
    try
    {
        const nlohmann::json j = nlohmann::json::parse(resp.body);
        if(!j.contains("models") || !j["models"].is_array())
            return names;
        for(const auto& item : j["models"])
        {
            const std::string name = item.value("name", item.value("model", ""));
            if(!name.empty())
                names.push_back(name);
        }
    }
    catch(...)
    {
    }
    return names;
}

std::string formatOllamaModelMismatchHint(const LLMProviderProfile& profile)
{
    const auto models = listOllamaTagModels(profile);
    std::ostringstream oss;
    oss << " (tip: AI Assistant → Settings → set Model to a name from `ollama list`; ";
    oss << "Base URL e.g. " << profile.base_url << "; configured model: " << profile.model;
    if(!models.empty())
    {
        oss << "; available on server:";
        for(size_t i = 0; i < models.size() && i < 6; ++i)
            oss << " " << models[i];
        if(models.size() > 6)
            oss << " …";
    }
    else
    {
        oss << "; could not list models at " << ollamaHostKey(profile) << "/api/tags";
    }
    oss << ")";
    return oss.str();
}

void applyOllamaEnvironmentDefaults(LLMProviderProfile& profile,
                                    const LLMRuntimeProviderSettings& runtime)
{
    if(profile.kind != LLMProviderKind::OllamaOpenAICompat
       && profile.kind != LLMProviderKind::OllamaNative)
        return;

    const auto it = runtime.endpoint_overrides_by_profile_id.find(profile.profile_id);
    const bool has_url =
        it != runtime.endpoint_overrides_by_profile_id.end() && !it->second.base_url.empty();
    const bool has_model =
        it != runtime.endpoint_overrides_by_profile_id.end() && !it->second.model.empty();

    if(const char* url = std::getenv("NMSDK_LLM_OLLAMA_BASE_URL"); url && *url && !has_url)
    {
        profile.base_url = url;
        if(profile.kind == LLMProviderKind::OllamaOpenAICompat
           && profile.base_url.find("/v1") == std::string::npos)
            profile.base_url += "/v1";
    }
    if(const char* model = std::getenv("NMSDK_LLM_OLLAMA_MODEL"); model && *model && !has_model)
        profile.model = model;
}

} // namespace RDK::LLM
