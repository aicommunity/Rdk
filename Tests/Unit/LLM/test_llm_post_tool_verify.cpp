#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMStepVerifier.h"

using namespace RDK::LLM;

TEST(LLMPostToolVerify, BuildCriteriaAddComponent)
{
    nlohmann::json args = {{"class_name", "PulseNeuron"},
                           {"parent_long_name", "Model/Zone"},
                           {"short_name", "N1"}};
    ToolGatewayResult tr;
    tr.ok = true;
    tr.result["long_name"] = "Model/Zone/N1";

    const SuccessCriteria c = buildPostVerifyCriteria("add_component", args, tr);
    EXPECT_EQ(c.type, "component_under_parent");
    EXPECT_EQ(c.params["class_name"], "PulseNeuron");
    EXPECT_EQ(c.params["parent_long_name"], "Model/Zone");
    EXPECT_EQ(c.params["long_name"], "Model/Zone/N1");
    EXPECT_EQ(c.params["short_name"], "N1");
}

TEST(LLMPostToolVerify, BuildCriteriaConnectComponents)
{
    nlohmann::json args = {{"from_long_name", "Model/A"},
                           {"from_property", "Out"},
                           {"to_long_name", "Model/B"},
                           {"to_property", "In"}};
    ToolGatewayResult tr;
    tr.ok = true;

    const SuccessCriteria c = buildPostVerifyCriteria("connect_components", args, tr);
    EXPECT_EQ(c.type, "link_exists");
    EXPECT_EQ(c.params["from_long_name"], "Model/A");
    EXPECT_EQ(c.params["to_property"], "In");
}

TEST(LLMPostToolVerify, SkipsNonWriteTools)
{
    URdkDomainAccess domain(nullptr);
    ToolGatewayResult tr;
    tr.ok = true;
    const VerifyResult r =
        runPostToolVerification("set_property", nlohmann::json::object(), tr, domain, 0);
    EXPECT_TRUE(r.satisfied);
    EXPECT_EQ(r.detail, "post_verify_skipped");
}

TEST(LLMPostToolVerify, GatewayNotOkSkipsSatisfied)
{
    URdkDomainAccess domain(nullptr);
    ToolGatewayResult tr;
    tr.ok = false;
    const VerifyResult r = runPostToolVerification(
        "add_component",
        {{"class_name", "X"}, {"parent_long_name", "Model"}, {"short_name", "a"}}, tr, domain, 0);
    EXPECT_FALSE(r.satisfied);
    EXPECT_EQ(r.detail, "gateway_not_ok");
}

TEST(LLMPostToolVerify, ConnectMissingEndpointReported)
{
    URdkDomainAccess domain(nullptr);
    ToolGatewayResult tr;
    tr.ok = true;
    nlohmann::json args = {{"from_long_name", "Model/Missing"},
                           {"from_property", "Out"},
                           {"to_long_name", "Model/B"},
                           {"to_property", "In"}};
    const VerifyResult r = runPostToolVerification("connect_components", args, tr, domain, 0);
    EXPECT_FALSE(r.satisfied);
    EXPECT_NE(r.detail.find("from_component_missing"), std::string::npos);
}
