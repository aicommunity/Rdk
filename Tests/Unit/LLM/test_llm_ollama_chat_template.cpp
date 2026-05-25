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

    const auto prepared = ensureRdkSystemPrompt(std::move(msgs));
    ASSERT_EQ(prepared.size(), 2u);
    EXPECT_EQ(prepared.front().role, LLMMessage::Role::System);
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
