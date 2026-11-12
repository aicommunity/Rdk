#include <gtest/gtest.h>

#include "Providers/ULLMThinkingParse.h"
#include "Providers/UOllamaChatTemplate.h"
#include "Orchestrator/ULLMToolFilterExpand.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMSearchTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"

using namespace RDK::LLM;

TEST(LLMThinkingParse, SplitThinkTagsFromContent)
{
    std::string text = "<think>reason here</think>\nFinal answer";
    std::string thinking;
    splitThinkTagsFromContent(text, thinking);
    EXPECT_EQ(thinking, "reason here");
    EXPECT_EQ(text, "Final answer");
}

TEST(LLMThinkingParse, ExtractThinkingAndReasoningContent)
{
    nlohmann::json msg = {{"thinking", "step1"}, {"content", "hi"}};
    std::string thinking;
    extractThinkingFields(msg, thinking);
    EXPECT_EQ(thinking, "step1");

    nlohmann::json msg2 = {{"reasoning_content", "r2"}, {"content", "x"}};
    thinking.clear();
    extractThinkingFields(msg2, thinking);
    EXPECT_EQ(thinking, "r2");

    // qwen3 via OpenAI-compat /v1 emits "reasoning"
    nlohmann::json msg3 = {{"reasoning", "r3"}, {"content", ""}};
    thinking.clear();
    extractThinkingFields(msg3, thinking);
    EXPECT_EQ(thinking, "r3");
}

TEST(LLMThinkingParse, FinalizeThinkingFromTags)
{
    LLMCompletionResult r;
    r.text = "<think>plan</think>done";
    finalizeThinkingResult(r);
    EXPECT_EQ(r.thinking, "plan");
    EXPECT_EQ(r.text, "done");
}

TEST(LLMThinkingParse, MessageJsonPreservesThinking)
{
    LLMMessage m;
    m.role = LLMMessage::Role::Assistant;
    m.content = "";
    m.thinking = "why call tool";
    LLMToolCall call;
    call.id = "1";
    call.name = "get_net_snapshot";
    call.arguments = nlohmann::json::object();
    m.assistant_tool_calls = std::vector<LLMToolCall>{call};

    const nlohmann::json msgs = buildOpenAiChatMessagesJson({m});
    ASSERT_EQ(msgs.size(), 1u);
    EXPECT_EQ(msgs[0]["thinking"], "why call tool");
    EXPECT_TRUE(msgs[0].contains("tool_calls"));
}

TEST(LLMThinkingParse, ToolArgumentsObjectOrString)
{
    nlohmann::json fn_obj = {{"name", "x"}, {"arguments", {{"path", "/tmp"}}}};
    EXPECT_EQ(parseToolCallArgumentsJson(fn_obj)["path"], "/tmp");

    nlohmann::json fn_str = {{"name", "x"}, {"arguments", "{\"a\":1}"}};
    EXPECT_EQ(parseToolCallArgumentsJson(fn_str)["a"], 1);
}

TEST(LLMToolFilterExpand, SearchToolsExpandsAllowlist)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    RegisterAgentTools(registry);

    ToolFilter filter;
    filter.include_write = false;
    filter.allowed_tool_names = std::unordered_set<std::string>{"search_tools", "ask_user"};

    SearchToolsResult found;
    found.query = "snapshot";
    found.tools = {"get_net_snapshot", "search_project_docs"};
    const nlohmann::json payload = enrichSearchToolsPayload(found, registry);
    ASSERT_TRUE(payload["tools"].is_array());
    EXPECT_TRUE(payload["tools"][0].contains("description"));

    expandToolFilterFromSearchResult(filter, payload, registry);
    ASSERT_TRUE(filter.allowed_tool_names.has_value());
    EXPECT_TRUE(filter.allowed_tool_names->count("get_net_snapshot") > 0);
    EXPECT_TRUE(filter.allowed_tool_names->count("search_project_docs") > 0);

    const auto exposed = registry.listForLlmApi(filter);
    std::unordered_set<std::string> names;
    for(const auto& d : exposed)
        names.insert(d.name);
    EXPECT_TRUE(names.count("get_net_snapshot") > 0);
}
