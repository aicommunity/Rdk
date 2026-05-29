#include <gtest/gtest.h>

#include "Context/ULinkPatternCatalog.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMConnectPlanBuilder.h"
#include "Orchestrator/ULLMConnectPlanParsing.h"
#include "Session/ULLMConversationStore.h"
#include "Session/ULLMSessionGraphMemory.h"

using namespace RDK::LLM;

namespace {

ConversationState makeStateWithAdded(std::vector<std::string> added)
{
    ConversationState st;
    st.session_id = "test";
    st.session_graph.added_long_names = std::move(added);
    return st;
}

} // namespace

TEST(LLMConnectPlanBuilder, RemainingWithOddCountFails)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st = makeStateWithAdded({"/ch0/A", "/ch0/B", "/ch0/C"});

    ConnectPlanBuildRequest req{"connect remaining",
                                parseConnectGoal("connect remaining"),
                                LLMSessionContext{},
                                &st,
                                domain,
                                catalog,
                                1,
                                nlohmann::json{{"components", nlohmann::json::array()}, {"links", nlohmann::json::array()}}};
    req.session.active_channel_index = 0;

    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    EXPECT_FALSE(r.ok);
}

TEST(LLMConnectPlanBuilder, NoPreviousTemplateWhenAnalogous)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st = makeStateWithAdded({"/ch0/A", "/ch0/B"});
    ParsedConnectGoal parsed = parseConnectGoal("connect like previous");
    parsed.wants_analogous = true;

    ConnectPlanBuildRequest req{"connect like previous",
                                parsed,
                                LLMSessionContext{},
                                &st,
                                domain,
                                catalog,
                                1,
                                nlohmann::json{{"components", nlohmann::json::array()}, {"links", nlohmann::json::array()}}};
    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    EXPECT_FALSE(r.ok);
    ASSERT_FALSE(r.issues.empty());
    EXPECT_EQ(r.issues.front(), "no_previous_template");
}

TEST(LLMConnectPlanBuilder, InsufficientNewLinksAfterSnapshotSkip)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st = makeStateWithAdded(
        {"/ch0/MModel.A", "/ch0/MModel.B", "/ch0/MModel.C", "/ch0/MModel.D"});
    st.session_graph.last_template = ConnectTemplate{"NSPNeuron", "NSPNeuron", "Output", "Input"};

    ParsedConnectGoal parsed = parseConnectGoal("link 2 pairs");
    ASSERT_EQ(parsed.kind, ConnectGoalKind::CountOnly);
    parsed.wants_analogous = true;

    const nlohmann::json snap = {
        {"components",
         nlohmann::json::array({{{"long_name", "/ch0/MModel.A"},
                                 {"short_name", "A"},
                                 {"class_name", "NSPNeuron"}},
                                {{"long_name", "/ch0/MModel.B"},
                                 {"short_name", "B"},
                                 {"class_name", "NSPNeuron"}},
                                {{"long_name", "/ch0/MModel.C"},
                                 {"short_name", "C"},
                                 {"class_name", "NSPNeuron"}},
                                {{"long_name", "/ch0/MModel.D"},
                                 {"short_name", "D"},
                                 {"class_name", "NSPNeuron"}}})},
        {"links",
         nlohmann::json::array({{{"from_long_name", "/ch0/MModel.A"},
                                 {"from_property", "Output"},
                                 {"to_long_name", "/ch0/MModel.B"},
                                 {"to_property", "Input"}}})}};

    ConnectPlanBuildRequest req{"link 2 pairs",
                                parsed,
                                LLMSessionContext{},
                                &st,
                                domain,
                                catalog,
                                1,
                                std::optional<nlohmann::json>(snap)};
    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    EXPECT_FALSE(r.ok);
    EXPECT_TRUE(std::find(r.issues.begin(), r.issues.end(), "insufficient_new_links")
                != r.issues.end());
}

