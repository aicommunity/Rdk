#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMStepVerifier.h"

using namespace RDK::LLM;

TEST(LLMStepVerifier, MultiLinkExistsEmptyIsFalse)
{
    URdkDomainAccess domain(nullptr);
    SuccessCriteria c;
    c.type = "multi_link_exists";
    c.params = {{"links", nlohmann::json::array()}};
    VerifyResult r = verifySuccessCriteria(c, domain, 0);
    EXPECT_FALSE(r.satisfied);
}

TEST(LLMStepVerifier, BuildPostVerifyCriteriaUnknownToolEmpty)
{
    ToolGatewayResult tr;
    tr.ok = true;
    const SuccessCriteria c = buildPostVerifyCriteria("save_configuration", nlohmann::json::object(), tr);
    EXPECT_TRUE(c.type.empty());
}

TEST(LLMStepVerifier, AddComponentVerifyUsesGatewayLongNameOnly)
{
    URdkDomainAccess domain(nullptr);
    ToolGatewayResult tr;
    tr.ok = true;
    tr.result["long_name"] = "Model.Zone.Neuron1";
    const nlohmann::json args = {{"short_name", "PNeuron"}, {"class_name", "NSPNeuron"},
                                 {"parent_long_name", "Model.Zone"}};
    const SuccessCriteria c = buildPostVerifyCriteria("add_component", args, tr);
    VerifyResult vr = verifySuccessCriteria(c, domain, 0);
    EXPECT_FALSE(vr.satisfied);
    EXPECT_NE(vr.detail.find("Model.Zone.Neuron1"), std::string::npos);
}

