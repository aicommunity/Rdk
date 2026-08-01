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

TEST(LLMConnectPlanBuilder, LiveAnalogousFanOutNestedPorts)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st;
    st.session_id = "test";

    const std::string goal =
        "подключи PGenerator ко всем нейронам так же как он подключен к PNeuron";
    ParsedConnectGoal parsed = parseConnectGoal(goal);
    ASSERT_TRUE(parsed.analogous_ref_token.has_value());
    ASSERT_TRUE(parsed.hub_token.has_value());

    const nlohmann::json snap = {
        {"components",
         nlohmann::json::array(
             {{{"long_name", "PGenerator"}, {"short_name", "PGenerator"}, {"class_name", "NSPGen"}},
              {{"long_name", "PNeuron"},
               {"short_name", "PNeuron"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuron.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronGen2"},
               {"short_name", "PNeuronGen2"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen2.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronGen3"},
               {"short_name", "PNeuronGen3"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen3.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}}})},
        {"links",
         nlohmann::json::array({{{"from_long_name", "PGenerator"},
                                 {"from_property", "Output"},
                                 {"to_long_name", "PNeuron.Soma1.ExcSynapse1"},
                                 {"to_property", "Input"}}})}};

    ConnectPlanBuildRequest req{goal, parsed, LLMSessionContext{}, &st, domain, catalog, 1,
                                std::optional<nlohmann::json>(snap)};
    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    ASSERT_TRUE(r.ok) << (r.issues.empty() ? "" : r.issues.front());
    ASSERT_EQ(r.steps.size(), 2u);

    auto has_step_to = [&](const std::string& to_ln) {
        for(const auto& step : r.steps)
        {
            if(step.arguments.value("to_long_name", "") == to_ln
               && step.arguments.value("from_long_name", "") == "PGenerator"
               && step.arguments.value("from_property", "") == "Output"
               && step.arguments.value("to_property", "") == "Input")
                return true;
        }
        return false;
    };
    EXPECT_TRUE(has_step_to("PNeuronGen2.Soma1.ExcSynapse1"));
    EXPECT_TRUE(has_step_to("PNeuronGen3.Soma1.ExcSynapse1"));
}

TEST(LLMConnectPlanBuilder, LiveAnalogousNoTemplateLinks)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st;
    st.session_id = "test";

    ParsedConnectGoal parsed = parseConnectGoal(
        "подключи PGenerator ко всем нейронам так же как он подключен к PNeuron");
    const nlohmann::json snap = {
        {"components",
         nlohmann::json::array(
             {{{"long_name", "PGenerator"}, {"short_name", "PGenerator"}, {"class_name", "NSPGen"}},
              {{"long_name", "PNeuron"},
               {"short_name", "PNeuron"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen2"},
               {"short_name", "PNeuronGen2"},
               {"class_name", "NSPNeuronGen"}}})},
        {"links", nlohmann::json::array()}};

    ConnectPlanBuildRequest req{"goal", parsed, LLMSessionContext{}, &st, domain, catalog, 1,
                                std::optional<nlohmann::json>(snap)};
    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    EXPECT_FALSE(r.ok);
    EXPECT_TRUE(std::find(r.issues.begin(), r.issues.end(), "no_template_links") != r.issues.end());
}

TEST(LLMConnectPlanBuilder, LiveAnalogousSessionPeers)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    ConversationState st;
    st.session_id = "test";
    st.session_graph.added_long_names = {"PNeuronGen", "PNeuronGen2", "PNeuronGen3"};

    ParsedConnectGoal parsed =
        parseConnectGoal("подключил PGenerator к этим нейронам также как к PNeuron");
    ASSERT_TRUE(parsed.wants_session_peers);

    const nlohmann::json snap = {
        {"components",
         nlohmann::json::array(
             {{{"long_name", "PGenerator"}, {"short_name", "PGenerator"}, {"class_name", "NSPGen"}},
              {{"long_name", "PNeuron"},
               {"short_name", "PNeuron"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuron.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronGen"},
               {"short_name", "PNeuronGen"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronGen2"},
               {"short_name", "PNeuronGen2"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen2.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronGen3"},
               {"short_name", "PNeuronGen3"},
               {"class_name", "NSPNeuronGen"}},
              {{"long_name", "PNeuronGen3.Soma1.ExcSynapse1"},
               {"short_name", "ExcSynapse1"},
               {"class_name", "NSPSynapse"}},
              {{"long_name", "PNeuronOther"},
               {"short_name", "PNeuronOther"},
               {"class_name", "NSPNeuronGen"}}})},
        {"links",
         nlohmann::json::array({{{"from_long_name", "PGenerator"},
                                 {"from_property", "Output"},
                                 {"to_long_name", "PNeuron.Soma1.ExcSynapse1"},
                                 {"to_property", "Input"}}})}};

    ConnectPlanBuildRequest req{"goal", parsed, LLMSessionContext{}, &st, domain, catalog, 1,
                                std::optional<nlohmann::json>(snap)};
    const ConnectPlanBuildResult r = buildConnectPlanSteps(req);
    ASSERT_TRUE(r.ok) << (r.issues.empty() ? "" : r.issues.front());
    ASSERT_EQ(r.steps.size(), 3u);
    for(const auto& step : r.steps)
    {
        const std::string to = step.arguments.value("to_long_name", "");
        EXPECT_EQ(to.find("PNeuronOther"), std::string::npos);
        EXPECT_TRUE(to == "PNeuronGen.Soma1.ExcSynapse1" || to == "PNeuronGen2.Soma1.ExcSynapse1"
                    || to == "PNeuronGen3.Soma1.ExcSynapse1");
    }
}

