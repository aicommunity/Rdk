#include <gtest/gtest.h>

#include "Domain/ULLMModelLinkWalker.h"
#include "Domain/URdkDomainAccess.h"

using namespace RDK::LLM;

TEST(LLMModelLinkWalker, ApplyLinkPagePagination)
{
    std::vector<LinkQuad> all = {
        {"/ch0/A", "Out", "/ch0/B", "In"},
        {"/ch0/C", "Out", "/ch0/D", "In"},
        {"/ch0/E", "Out", "/ch0/F", "In"},
        {"/ch0/G", "Out", "/ch0/H", "In"},
        {"/ch0/I", "Out", "/ch0/J", "In"},
    };

    ModelLinkWalkOptions opts;
    opts.offset = 2;
    opts.limit = 2;
    const ModelLinkWalkResult page = applyLinkPage(all, opts);

    ASSERT_EQ(page.links.size(), 2u);
    EXPECT_EQ(page.links[0].from_long_name, "/ch0/E");
    EXPECT_EQ(page.links[1].from_long_name, "/ch0/G");
    EXPECT_TRUE(page.truncated);
    EXPECT_EQ(page.next_offset, 4);
}

TEST(LLMModelLinkWalker, ApplyLinkPageStopOnMatch)
{
    std::vector<LinkQuad> all = {
        {"/ch0/A", "Out", "/ch0/B", "In"},
        {"/ch0/C", "Out", "/ch0/D", "In"},
    };

    ModelLinkWalkOptions opts;
    opts.match_quad = LinkQuad{"/ch0/C", "Out", "/ch0/D", "In"};
    opts.stop_on_first_match = true;
    const ModelLinkWalkResult page = applyLinkPage(all, opts);

    EXPECT_TRUE(page.found_match);
}

TEST(LLMModelLinkWalker, DedupSameQuad)
{
    const LinkQuad quad{"/ch0/A", "Out", "/ch0/B", "In"};
    const std::vector<LinkQuad> raw = {quad, quad, {"/ch0/C", "Out", "/ch0/D", "In"}};
    const std::vector<LinkQuad> deduped = dedupeModelLinkQuads(raw);
    ASSERT_EQ(deduped.size(), 2u);
    EXPECT_EQ(deduped[0].from_long_name, "/ch0/A");
    EXPECT_EQ(deduped[1].from_long_name, "/ch0/C");
}

TEST(LLMModelLinkWalker, PlanSnapshotHasLinkWhenTruncated)
{
    const LinkQuad quad{"/ch0/MModel.A", "Output", "/ch0/MModel.B", "Input"};
    const nlohmann::json snap = {
        {"links",
         nlohmann::json::array({{{"from_long_name", "/ch0/MModel.A"},
                                 {"from_property", "Output"},
                                 {"to_long_name", "/ch0/MModel.B"},
                                 {"to_property", "Input"}}})},
        {"links_truncated", true}};
    URdkDomainAccess domain(nullptr);
    EXPECT_TRUE(planSnapshotOrModelHasLink(snap, domain, quad, 0));
}

TEST(LLMModelLinkWalker, ListModelLinksNotInitialized)
{
    URdkDomainAccess domain(nullptr);
    nlohmann::json out;
    const DomainStatus st = domain.listModelLinks(out, 0, "", 0, 10);
    EXPECT_FALSE(st.ok());
}
