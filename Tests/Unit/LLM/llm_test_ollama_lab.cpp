#include "llm_test_ollama_lab.h"

#include <gtest/gtest.h>

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

void skipIfLabOllamaUnreachable()
{
    if(!isLabOllamaReachable())
        GTEST_SKIP() << "Lab Ollama unreachable at " << kLabOllamaHost;
}

} // namespace RDK::LLM::Test
