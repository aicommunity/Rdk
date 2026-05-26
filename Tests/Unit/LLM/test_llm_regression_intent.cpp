#include <algorithm>
#include <gtest/gtest.h>

#include "Orchestrator/ULLMIntentParser.h"
#include "Orchestrator/ULLMToolFilterBuilder.h"
#include "Tools/ULLMToolRegistry.h"

TEST(LLMRegressionIntent, ListComponentsPhraseIsQuery)
{
    const RDK::LLM::IntentParseResult r =
        RDK::LLM::ULLMIntentParser{}.parseDetailed("какие компоненты на схеме");
    EXPECT_EQ(r.kind, RDK::LLM::LLMIntentKind::Query);
}

TEST(LLMToolRegistry, DisconnectNotExposedToProvider)
{
    RDK::LLM::ULLMToolRegistry registry;
    RDK::LLM::LLMToolDefinition d;
    d.name = "disconnect_components";
    d.kind = RDK::LLM::LLMToolKind::Write;
    d.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(d, [](const nlohmann::json&) {
        RDK::LLM::ToolGatewayResult r;
        r.ok = true;
        return r;
    });
    RDK::LLM::ToolFilter filter;
    filter.include_write = true;
    const auto list = registry.listForLlmApi(filter);
    EXPECT_TRUE(std::none_of(list.begin(), list.end(),
                             [](const RDK::LLM::LLMToolDefinition& t) {
                                 return t.name == "disconnect_components";
                             }));
}

TEST(LLMToolFilter, QueryExcludesWriteTools)
{
    RDK::LLM::ULLMToolRegistry registry;
    RDK::LLM::LLMToolDefinition read_def;
    read_def.name = "search_project_docs";
    read_def.kind = RDK::LLM::LLMToolKind::Read;
    read_def.input_schema = {{"type", "object"}};
    registry.registerTool(read_def, [](const nlohmann::json&) {
        RDK::LLM::ToolGatewayResult r;
        r.ok = true;
        return r;
    });

    RDK::LLM::LLMToolDefinition write_def;
    write_def.name = "add_component";
    write_def.kind = RDK::LLM::LLMToolKind::Write;
    write_def.input_schema = {{"type", "object"}};
    registry.registerTool(write_def, [](const nlohmann::json&) {
        RDK::LLM::ToolGatewayResult r;
        r.ok = true;
        return r;
    });

    const RDK::LLM::ToolFilter filter = RDK::LLM::buildToolFilter(
        RDK::LLM::LLMIntentKind::Query, false, RDK::LLM::ConfigurationLifecycleAction::None);
    const auto tools = registry.buildOpenAiToolsJson(filter);

    EXPECT_EQ(tools.size(), 1u);
    EXPECT_EQ(tools.front()["function"]["name"].get<std::string>(), "search_project_docs");
}
