#include <gtest/gtest.h>

#include "Observability/ULLMToolTrace.h"
#include "Orchestrator/ULLMTaskOutcomeSummary.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMTaskOutcomeSummary, ConnectLineUsesShortNamesAndPorts)
{
    const nlohmann::json args = {
        {"from_long_name", "Model/PGenerator"},
        {"from_property", "Output"},
        {"to_long_name", "Model/PNeuronGen1/ExcSynapse1"},
        {"to_property", "Input"},
    };
    const std::string line = formatConnectLinkOutcomeLine(args);
    EXPECT_NE(line.find("PGenerator.Output"), std::string::npos);
    EXPECT_NE(line.find("PNeuronGen1/ExcSynapse1.Input"), std::string::npos);
    EXPECT_NE(line.find("→"), std::string::npos);
}

TEST(LLMTaskOutcomeSummary, SummaryListsConnectQuadsNotOnlyStepCount)
{
    const std::vector<std::string> links = {
        formatConnectLinkOutcomeLine({{"from_long_name", "Model/PGenerator"},
                                      {"from_property", "Output"},
                                      {"to_long_name", "Model/PNeuronGen1/ExcSynapse1"},
                                      {"to_property", "Input"}}),
        formatConnectLinkOutcomeLine({{"from_long_name", "Model/PGenerator"},
                                      {"from_property", "Output"},
                                      {"to_long_name", "Model/PNeuronGen2/ExcSynapse1"},
                                      {"to_property", "Input"}}),
        formatConnectLinkOutcomeLine({{"from_long_name", "Model/PGenerator"},
                                      {"from_property", "Output"},
                                      {"to_long_name", "Model/PNeuronGen3/ExcSynapse1"},
                                      {"to_property", "Input"}}),
    };
    const std::string summary =
        buildTaskExecuteSummary(links, {}, "task_demo", 5, true, "");
    EXPECT_NE(summary.find("Connected 3 link(s):"), std::string::npos);
    EXPECT_NE(summary.find("PGenerator.Output"), std::string::npos);
    EXPECT_NE(summary.find("PNeuronGen1/ExcSynapse1.Input"), std::string::npos);
    EXPECT_NE(summary.find("PNeuronGen2/ExcSynapse1.Input"), std::string::npos);
    EXPECT_NE(summary.find("PNeuronGen3/ExcSynapse1.Input"), std::string::npos);
    EXPECT_EQ(summary.find("5 steps completed."), std::string::npos);
    EXPECT_NE(summary.find("task_demo: 5 steps"), std::string::npos);
}

TEST(LLMTaskOutcomeSummary, AddLinesKeepAddedWording)
{
    const nlohmann::json args = {{"class_name", "UEPulseNeuron"}, {"short_name", "PNeuron"}};
    const nlohmann::json result = {{"class_name", "UEPulseNeuron"},
                                   {"long_name", "Model/PNeuron"}};
    const std::string line = formatAddComponentOutcomeLine(args, result);
    const std::string summary = buildTaskExecuteSummary({}, {line}, "task_add", 1, true, "");
    EXPECT_NE(summary.find("Added 1 component(s):"), std::string::npos);
    EXPECT_NE(summary.find("UEPulseNeuron"), std::string::npos);
    EXPECT_NE(summary.find("PNeuron"), std::string::npos);
}

TEST(LLMTaskOutcomeSummary, ToolTraceGrowsPerConnectStep)
{
    ConversationState state;
    state.session_id = "s1";
    const int connect_steps = 3;
    for(int i = 0; i < connect_steps; ++i)
    {
        ToolGatewayResult tr;
        tr.ok = true;
        tr.result = {{"ok", true}};
        const nlohmann::json args = {
            {"from_long_name", "Model/PGenerator"},
            {"from_property", "Output"},
            {"to_long_name", "Model/PNeuronGen" + std::to_string(i + 1) + "/ExcSynapse1"},
            {"to_property", "Input"},
        };
        recordTurnToolInvocation(state, "connect_components", args, tr);
    }
    EXPECT_GE(static_cast<int>(state.current_turn_tool_trace.size()), connect_steps);
    const std::string html = formatTurnToolTraceHtml(state.current_turn_tool_trace);
    EXPECT_NE(html.find("connect_components"), std::string::npos);
    EXPECT_NE(html.find("from_property"), std::string::npos);
    EXPECT_NE(html.find("to_long_name"), std::string::npos);
}
