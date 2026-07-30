#include "llm_test_ollama_lab.h"

#include <cstdlib>

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
    return "qwen3";
}

bool labHasThinkingModel()
{
    ULLMHttpClient http;
    const auto resp = http.get(std::string(kLabOllamaHost) + "/api/tags", "", 5000);
    if(resp.status_code < 200 || resp.status_code >= 300 || !resp.error.empty())
        return false;
    const std::string model = labOllamaThinkingModelName();
    if(resp.body.find("\"" + model + "\"") != std::string::npos)
        return true;
    if(resp.body.find("\"" + model + ":") != std::string::npos)
        return true;
    // Common family default for thinking profile.
    if(model == "qwen3" && resp.body.find("qwen3") != std::string::npos)
        return true;
    return false;
}

} // namespace RDK::LLM::Test
