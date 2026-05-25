#include <algorithm>
#include <gtest/gtest.h>

#include "Orchestrator/ULLMIntentParser.h"
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
