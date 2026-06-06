#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include <cstdlib>

#include "Http/ULLMHttpClient.h"
#include "Providers/ULLMProviderFactory.h"
#include "llm_test_ollama_lab.h"

using namespace RDK::LLM;
using namespace RDK::LLM::Test;

TEST(LabOllamaIntegration, ApiTagsListsModels)
{
    skipIfLabOllamaUnreachable();

    ULLMHttpClient http;
    const auto resp = http.get(std::string(kLabOllamaHost) + "/api/tags", "", 8000);
    ASSERT_GE(resp.status_code, 200);
    ASSERT_LT(resp.status_code, 300) << resp.body;
    ASSERT_TRUE(resp.error.empty()) << resp.error;

    const nlohmann::json j = nlohmann::json::parse(resp.body);
    ASSERT_TRUE(j.contains("models"));
    ASSERT_TRUE(j["models"].is_array());
    ASSERT_FALSE(j["models"].empty());
}

TEST(LabOllamaIntegration, OpenAiCompatChatCompletion)
{
    skipIfLabOllamaUnreachable();

    const std::string model = labOllamaModelName();
    nlohmann::json body = {{"model", model},
                           {"messages",
                            nlohmann::json::array({{{"role", "user"},
                                                    {"content", "Reply with exactly: OK"}}})},
                           {"max_tokens", 16},
                           {"temperature", 0.0}};

    ULLMHttpClient http;
    const auto resp = http.postJson(std::string(kLabOllamaOpenAiV1) + "/chat/completions",
                                    body.dump(), "", 120000);
    if(resp.status_code == 404)
        GTEST_SKIP() << "Model not found on lab Ollama: " << model << " — " << resp.body;
    ASSERT_GE(resp.status_code, 200) << resp.body;
    ASSERT_LT(resp.status_code, 300) << resp.body;

    const nlohmann::json j = nlohmann::json::parse(resp.body);
    ASSERT_TRUE(j.contains("choices"));
    ASSERT_FALSE(j["choices"].empty());
    EXPECT_TRUE(j["choices"][0].contains("message"));
}

TEST(LabOllamaIntegration, ProviderHealthCheck)
{
    skipIfLabOllamaUnreachable();

    LLMProviderProfile profile;
    profile.kind = LLMProviderKind::OllamaOpenAICompat;
    profile.base_url = kLabOllamaOpenAiV1;
    profile.model = labOllamaModelName();

    std::unique_ptr<ILLMProvider> provider = ULLMProviderFactory::create(profile);
    ASSERT_NE(provider, nullptr);

    std::string err;
    if(!provider->healthCheck(err))
        GTEST_SKIP() << "healthCheck failed: " << err;

    std::vector<LLMMessage> messages;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content =
        "What is a NeuroModeler configuration (project.ini)? Answer in one short sentence.";
    messages.push_back(user);

    LLMCompletionOptions opts;
    opts.max_tokens = 128;
    opts.temperature = 0.1f;

    const LLMCompletionResult result = provider->chat(messages, opts);
    if(!result.ok && result.error_message.find("404") != std::string::npos)
        GTEST_SKIP() << "Model missing on lab host: " << profile.model;
    EXPECT_TRUE(result.ok) << result.error_message;
    EXPECT_FALSE(result.text.empty());
}

TEST(LabOllamaIntegration, ChatMentionsConfigurationTools)
{
    skipIfLabOllamaUnreachable();

    LLMProviderProfile profile;
    profile.kind = LLMProviderKind::OllamaOpenAICompat;
    profile.base_url = kLabOllamaOpenAiV1;
    profile.model = labOllamaModelName();

    std::unique_ptr<ILLMProvider> provider = ULLMProviderFactory::create(profile);
    ASSERT_NE(provider, nullptr);

    nlohmann::json tool_schema = {
        {"type", "object"},
        {"properties", {{"configuration_path", {{"type", "string"}}}}},
        {"required", nlohmann::json::array({"configuration_path"})},
        {"additionalProperties", false}};

    nlohmann::json tool_def = {{"type", "function"},
                               {"function",
                                {{"name", "validate_configuration"},
                                 {"description", "Validate a configuration file on disk"},
                                 {"parameters", tool_schema}}}};

    std::vector<LLMMessage> messages;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content =
        "I have project.ini at /tmp/example/project.ini. Which tool would validate it without "
        "opening the GUI?";
    messages.push_back(user);

    LLMCompletionOptions opts;
    opts.max_tokens = 256;
    opts.temperature = 0.0f;
    opts.tools_for_api = {tool_def};

    const LLMCompletionResult result = provider->chat(messages, opts);
    if(!result.ok)
    {
        if(result.error_message.find("404") != std::string::npos)
            GTEST_SKIP() << "Model missing: " << profile.model;
        if(result.error_message.find("tool") != std::string::npos
           || result.error_message.find("function") != std::string::npos)
            GTEST_SKIP() << "Lab model may not support tools: " << result.error_message;
    }

    EXPECT_TRUE(result.ok) << result.error_message;
    const bool mentions_validate = result.text.find("validate") != std::string::npos
                                   || result.text.find("validate_configuration")
                                          != std::string::npos;
    const bool tool_called =
        !result.tool_calls.empty()
        && result.tool_calls.front().name == "validate_configuration";
    EXPECT_TRUE(mentions_validate || tool_called)
        << "expected text or tool call about validate_configuration";
}
