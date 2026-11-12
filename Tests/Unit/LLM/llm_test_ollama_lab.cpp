#include "llm_test_ollama_lab.h"

#include <cstdlib>

#include <nlohmann/json.hpp>

#include "Http/ULLMHttpClient.h"

namespace RDK::LLM::Test {

bool isLabOllamaReachable(int timeout_ms)
{
    ULLMHttpClient http;
    const auto resp = http.get(std::string(kLabOllamaHost) + "/api/tags", "", timeout_ms);
    return resp.status_code >= 200 && resp.status_code < 300 && resp.error.empty();
}

std::string labOllamaModelName()
{
    if(const char* env = std::getenv("NMSDK_LLM_OLLAMA_MODEL"))
    {
        if(env[0] != '\0')
            return env;
    }
    return "qwen2.5:14b";
}

std::string labOllamaThinkingModelName()
{
    if(const char* env = std::getenv("NMSDK_LLM_OLLAMA_THINKING_MODEL"))
    {
        if(env[0] != '\0')
            return env;
    }
    return "qwen3:14b";
}

bool labHasThinkingModel()
{
    ULLMHttpClient http;
    const auto resp = http.get(std::string(kLabOllamaHost) + "/api/tags", "", 5000);
    if(resp.status_code < 200 || resp.status_code >= 300 || !resp.error.empty())
        return false;
    const std::string model = labOllamaThinkingModelName();
    const bool listed = resp.body.find("\"" + model + "\"") != std::string::npos
                        || resp.body.find("\"" + model + ":") != std::string::npos
                        || (model.find("qwen3") != std::string::npos && resp.body.find("qwen3") != std::string::npos);
    if(!listed)
        return false;

    // Probe think support: Ollama returns 400 "does not support thinking" for broken templates.
    const nlohmann::json body = {{"model", model},
                                 {"messages", nlohmann::json::array({{{"role", "user"},
                                                                      {"content", "ping"}}})},
                                 {"think", true},
                                 {"stream", false},
                                 {"options", {{"num_predict", 8}}}};
    const auto chat = http.postJson(std::string(kLabOllamaHost) + "/api/chat", body.dump(), "", 60000);
    if(!chat.error.empty())
        return false;
    if(chat.status_code < 200 || chat.status_code >= 300)
        return false;
    if(chat.body.find("does not support thinking") != std::string::npos)
        return false;
    return true;
}

} // namespace RDK::LLM::Test
