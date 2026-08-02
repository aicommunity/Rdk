#include <gtest/gtest.h>

#include "Providers/UOllamaChatTemplate.h"

using namespace RDK::LLM;

TEST(OllamaChatTemplate, DetectsQwenFromModelName)
{
    EXPECT_EQ(detectChatTemplateFamily("qwen2.5:7b"), OllamaChatTemplateFamily::Qwen2);
    EXPECT_EQ(detectChatTemplateFamily("llama3.2:latest"), OllamaChatTemplateFamily::Llama3);
}

TEST(OllamaChatTemplate, InjectsSystemPrompt)
{
    std::vector<LLMMessage> msgs;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "hello";
    msgs.push_back(user);

    const auto prepared = ensureRdkSystemPrompt(std::move(msgs), "en");
    ASSERT_EQ(prepared.size(), 2u);
    EXPECT_EQ(prepared.front().role, LLMMessage::Role::System);
}

TEST(OllamaChatTemplate, SystemPromptContainsLanguage)
{
    const std::string prompt = buildRdkSystemPrompt("ru");
    EXPECT_NE(prompt.find("ru"), std::string::npos);
    EXPECT_NE(prompt.find("Russian"), std::string::npos);
    EXPECT_NE(prompt.find("NeuroModeler"), std::string::npos);
    EXPECT_NE(prompt.find("robot"), std::string::npos); // anti-hallucination wording
    EXPECT_NE(prompt.find("update_configuration"), std::string::npos);
}

TEST(OllamaChatTemplate, EnsurePromptUsesLanguage)
{
    std::vector<LLMMessage> msgs;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "hello";
    msgs.push_back(user);

    const auto prepared = ensureRdkSystemPrompt(std::move(msgs), "de");
    ASSERT_EQ(prepared.size(), 2u);
    EXPECT_NE(prepared.front().content.find("German"), std::string::npos);
    EXPECT_NE(prepared.front().content.find("de"), std::string::npos);
}

TEST(OllamaChatTemplate, ManifestWithLanguageSkipsDuplicateRdkPrompt)
{
    std::vector<LLMMessage> msgs;
    LLMMessage manifest;
    manifest.role = LLMMessage::Role::System;
    manifest.content = "## Response language\nAlways respond in Russian (code: ru).\n## Tools";
    msgs.push_back(manifest);
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "hello";
    msgs.push_back(user);

    const auto prepared = ensureRdkSystemPrompt(std::move(msgs), "ru");
    EXPECT_EQ(prepared.size(), 2u);
    EXPECT_EQ(prepared.front().content.find("NeuroModeler AI assistant"), std::string::npos);
}

TEST(OllamaChatTemplate, SerializesAssistantToolCalls)
{
    std::vector<LLMMessage> msgs;
    LLMMessage assistant;
    assistant.role = LLMMessage::Role::Assistant;
    assistant.content = "";
    LLMToolCall call;
    call.id = "call_1";
    call.name = "get_net_snapshot";
    call.arguments = {{"channel_index", 0}};
    assistant.assistant_tool_calls = std::vector<LLMToolCall>{call};
    msgs.push_back(assistant);

    const nlohmann::json api = buildOpenAiChatMessagesJson(msgs);
    ASSERT_EQ(api.size(), 1);
    EXPECT_TRUE(api[0].contains("tool_calls"));
    EXPECT_EQ(api[0]["tool_calls"][0]["function"]["name"], "get_net_snapshot");
}

TEST(OllamaChatTemplate, EmbeddedPromptIncludesAssistantToolCallJson)
{
    std::vector<LLMMessage> msgs;
    LLMMessage assistant;
    assistant.role = LLMMessage::Role::Assistant;
    assistant.content = "";
    LLMToolCall call;
    call.name = "get_net_snapshot";
    call.arguments = {{"channel_index", 0}};
    assistant.assistant_tool_calls = std::vector<LLMToolCall>{call};
    msgs.push_back(assistant);

    const std::string prompt = formatPromptWithTemplate(OllamaChatTemplateFamily::Qwen2, msgs);
    EXPECT_NE(prompt.find("get_net_snapshot"), std::string::npos);
    EXPECT_NE(prompt.find("channel_index"), std::string::npos);
}

TEST(OllamaChatTemplate, QwenPromptContainsImStart)
{
    std::vector<LLMMessage> msgs;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "test";
    msgs.push_back(user);
    const std::string prompt = formatPromptWithTemplate(OllamaChatTemplateFamily::Qwen2, msgs);
    EXPECT_NE(prompt.find("<|im_start|>"), std::string::npos);
}
