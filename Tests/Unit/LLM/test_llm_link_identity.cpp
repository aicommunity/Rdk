#include <gtest/gtest.h>

#include "Domain/ULLMLinkIdentity.h"

using namespace RDK::LLM;

TEST(LLMLinkIdentity, QuadEqualsRequiresAllFourFields)
{
    const LinkQuad a{"/ch0/A", "Output", "/ch0/B", "Input"};
    const LinkQuad b{"/ch0/A", "Output", "/ch0/B", "Input"};
    const LinkQuad c{"/ch0/A", "Out2", "/ch0/B", "Input"};
    EXPECT_TRUE(linkQuadEquals(a, b));
    EXPECT_FALSE(linkQuadEquals(a, c));
}

TEST(LLMLinkIdentity, JsonMatchStrictQuad)
{
    const LinkQuad q{"/ch0/A", "Output", "/ch0/B", "Input"};
    const nlohmann::json link = {{"from_long_name", "/ch0/A"},
                                 {"from_property", "Output"},
                                 {"to_long_name", "/ch0/B"},
                                 {"to_property", "Input"}};
    EXPECT_TRUE(linkJsonMatchesQuad(link, q));
    const nlohmann::json partial = {{"from_long_name", "/ch0/A"},
                                    {"to_long_name", "/ch0/B"},
                                    {"from_property", "Output"},
                                    {"to_property", "Other"}};
    EXPECT_FALSE(linkJsonMatchesQuad(partial, q));
}

TEST(LLMLinkIdentity, SnapshotContainsLink)
{
    const LinkQuad q{"/ch0/A", "Output", "/ch0/B", "Input"};
    const nlohmann::json snap = {
        {"links",
         {{{"from_long_name", "/ch0/A"},
           {"from_property", "Output"},
           {"to_long_name", "/ch0/B"},
           {"to_property", "Input"}}}}};
    EXPECT_TRUE(snapshotContainsLink(snap, q));
}

TEST(LLMLinkIdentity, EndpointInSubtreeNested)
{
    EXPECT_TRUE(endpointInSubtree("PNeuron", "PNeuron"));
    EXPECT_TRUE(endpointInSubtree("PNeuron.Soma1.ExcSynapse1", "PNeuron"));
    EXPECT_FALSE(endpointInSubtree("PNeuronGen2.Soma1", "PNeuron"));
    EXPECT_FALSE(endpointInSubtree("PNeuronX", "PNeuron"));
}

TEST(LLMLinkIdentity, MapSubtreeEndpointBothDirections)
{
    EXPECT_EQ(mapSubtreeEndpoint("PNeuron.Soma1.In", "PNeuron", "PNeuronGen2"),
              "PNeuronGen2.Soma1.In");
    EXPECT_EQ(mapSubtreeEndpoint("PGenerator", "PNeuron", "PNeuronGen2"), "PGenerator");
    EXPECT_EQ(mapSubtreeEndpoint("PGenerator.OutChild", "PGenerator", "OtherGen"),
              "OtherGen.OutChild");
}

TEST(LLMLinkIdentity, FilterLinkQuadsBySubtree)
{
    const std::vector<LinkQuad> all = {
        {"PGenerator", "Output", "PNeuron.Soma1.ExcSynapse1", "Input"},
        {"PGenerator", "Output", "PNeuronGen2.Soma1.ExcSynapse1", "Input"},
        {"Other", "Out", "Else", "In"},
    };
    ModelLinkListFilters f;
    f.component_long_name = "PNeuron";
    const auto hit = filterLinkQuadsBySubtree(all, f);
    ASSERT_EQ(hit.size(), 1u);
    EXPECT_EQ(hit[0].to_long_name, "PNeuron.Soma1.ExcSynapse1");

    ModelLinkListFilters directed;
    directed.from_long_name = "PGenerator";
    directed.to_long_name = "PNeuron";
    const auto directed_hit = filterLinkQuadsBySubtree(all, directed);
    ASSERT_EQ(directed_hit.size(), 1u);
}
