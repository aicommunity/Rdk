#include <gtest/gtest.h>

#include "Orchestrator/ULLMAgentManifestBuilder.h"
#include "Orchestrator/ULLMConnectPlanParsing.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMAgentManifest, ResponseLanguageSection)
{
    ULLMToolRegistry registry;
    ToolFilter filter;
    const std::string manifest = buildAgentManifest(registry, filter, 8000, {}, {}, "ru");
    EXPECT_NE(manifest.find("## Response language"), std::string::npos);
    EXPECT_NE(manifest.find("Russian"), std::string::npos);
}

TEST(LLMAgentManifest, ConnectVsAddSection)
{
    ULLMToolRegistry registry;
    ToolFilter filter;
    const std::string manifest =
        buildAgentManifest(registry, filter, 8000, "подключи PNeuron2 к PNeuron3", {}, "en");
    EXPECT_NE(manifest.find("Connect vs Add"), std::string::npos);
    EXPECT_NE(manifest.find("connect_components"), std::string::npos);
    EXPECT_TRUE(isConnectGoalText("подключи PNeuron2 к PNeuron3"));
}
