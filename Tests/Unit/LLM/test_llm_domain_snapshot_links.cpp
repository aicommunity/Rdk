#include <gtest/gtest.h>

#include "Domain/ULLMLinkIdentity.h"

using namespace RDK::LLM;

TEST(LLMDomainSnapshotLinks, SnapshotLinksArrayMatchQuad)
{
    const nlohmann::json links = {
        {{"from_long_name", "/ch0/MModel.Neuron1"},
         {"from_property", "Output"},
         {"to_long_name", "/ch0/MModel.Neuron2"},
         {"to_property", "Input"}}};
    const LinkQuad q{"/ch0/MModel.Neuron1", "Output", "/ch0/MModel.Neuron2", "Input"};
    EXPECT_TRUE(linksArrayContains(links, q));
}

TEST(LLMDomainSnapshotLinks, ConnectResultAlreadyExistedFlag)
{
    const nlohmann::json result = {{"from_long_name", "/ch0/A"},
                                   {"from_property", "Output"},
                                   {"to_long_name", "/ch0/B"},
                                   {"to_property", "Input"},
                                   {"already_existed", true}};
    EXPECT_TRUE(result.value("already_existed", false));
    const LinkQuad q = linkQuadFromJson(result);
    EXPECT_EQ(q.from_long_name, "/ch0/A");
    EXPECT_EQ(q.from_property, "Output");
}
