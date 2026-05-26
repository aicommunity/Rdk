#include <gtest/gtest.h>

#include "Orchestrator/ULLMWriteToolUserMessage.h"

using namespace RDK::LLM;

TEST(LLMWriteToolUserMessage, ProviderJsonIncludesOkOnFailure)
{
    ToolGatewayResult tr;
    tr.ok = false;
    tr.error_code = "PROJECT_NOT_LOADED";
    tr.message = "No configuration is open";
    const nlohmann::json j = toolGatewayResultForProvider(tr);
    EXPECT_FALSE(j.at("ok").get<bool>());
    EXPECT_EQ(j.at("error_code").get<std::string>(), "PROJECT_NOT_LOADED");
}

TEST(LLMWriteToolUserMessage, ProviderJsonIncludesLongNameOnSuccess)
{
    ToolGatewayResult tr;
    tr.ok = true;
    tr.result["long_name"] = "Model/Neuron1";
    const nlohmann::json j = toolGatewayResultForProvider(tr);
    EXPECT_TRUE(j.at("ok").get<bool>());
    EXPECT_EQ(j.at("long_name").get<std::string>(), "Model/Neuron1");
}

TEST(LLMWriteToolUserMessage, FormatAddComponentSuccess)
{
    ToolGatewayResult tr;
    tr.ok = true;
    tr.result["long_name"] = "Model/PulseNeuron1";
    const std::string msg = formatWriteToolUserMessage("add_component", tr);
    EXPECT_NE(msg.find("Model/PulseNeuron1"), std::string::npos);
}
