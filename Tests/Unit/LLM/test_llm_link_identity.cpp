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
