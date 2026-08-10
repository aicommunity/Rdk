#include <gtest/gtest.h>

#include "Observability/ULLMToolTrace.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMToolTrace, SanitizeTruncatesLongStrings)
{
    std::string long_val(300, 'x');
    const nlohmann::json args = {{"comment", long_val}};
    const nlohmann::json out = sanitizeToolArgumentsForDisplay(args);
    ASSERT_TRUE(out["comment"].is_string());
    EXPECT_LT(out["comment"].get<std::string>().size(), 210u);
}

TEST(LLMToolTrace, SanitizeMasksSensitiveKeys)
{
    const nlohmann::json args = {{"api_key", "secret-value"}, {"class_name", "NSPNeuron"}};
    const nlohmann::json out = sanitizeToolArgumentsForDisplay(args);
    EXPECT_EQ(out["api_key"].get<std::string>(), "***");
    EXPECT_EQ(out["class_name"].get<std::string>(), "NSPNeuron");
}

TEST(LLMToolTrace, SanitizeNullSchemaPropertyDoesNotThrow)
{
    const nlohmann::json args = {{"password", "x"}, {"class_name", "NSPNeuron"}};
    nlohmann::json schema = {{"properties", {{"password", nullptr}, {"class_name", {{"type", "string"}}}}}};
    EXPECT_NO_THROW({
        const nlohmann::json out = sanitizeToolArgumentsForDisplay(args, schema);
        EXPECT_EQ(out["password"].get<std::string>(), "***");
        EXPECT_EQ(out["class_name"].get<std::string>(), "NSPNeuron");
    });
}

TEST(LLMToolTrace, FormatHtmlIncludesToolName)
{
    TurnToolInvocationView view;
    view.tool_name = "add_component";
    view.arguments = {{"class_name", "NSPNeuron"}, {"parent_long_name", "Model.Zone"}};
    view.ok = true;
    const std::string html = formatTurnToolTraceHtml({view});
    EXPECT_NE(html.find("add_component"), std::string::npos);
    EXPECT_NE(html.find("NSPNeuron"), std::string::npos);
    EXPECT_NE(html.find("<details>"), std::string::npos);
}

TEST(LLMToolTrace, RecordAppendsToConversationState)
{
    ConversationState state;
    ToolGatewayResult tr;
    tr.ok = true;
    recordTurnToolInvocation(state, "get_net_snapshot", {{"channel_index", 0}}, tr);
    ASSERT_EQ(state.current_turn_tool_trace.size(), 1u);
    EXPECT_EQ(state.current_turn_tool_trace.front().tool_name, "get_net_snapshot");
}
