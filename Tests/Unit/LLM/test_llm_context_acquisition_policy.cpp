#include <gtest/gtest.h>

#include "Orchestrator/ULLMContextAcquisitionPolicy.h"

using namespace RDK::LLM;

TEST(LLMContextAcquisitionPolicy, ComputePlanInjectsLinkPatternsWhenClassPair)
{
    ConversationState state;
    LLMSessionContext session;
    session.project_loaded = true;

    ContextAcquisitionSignals signals;
    signals.intent = LLMIntentKind::Mutate;
    signals.mutate_subkind = MutateSubkind::Connect;
    signals.from_class = "NSPNeuron";
    signals.to_class = "NSPNeuron";
    signals.has_resolved_component_pair = true;

    const ContextAcquisitionPlan plan =
        computeContextAcquisitionPlan(state, session, LLMGuiContextSnapshot{}, signals,
                                      LLMContextAcquisitionMode::Auto);
    EXPECT_TRUE(plan.inject_link_patterns);
    EXPECT_TRUE(plan.inject_connect_semantics);
}

TEST(LLMContextAcquisitionPolicy, MinimalModeDisablesInjection)
{
    ConversationState state;
    LLMSessionContext session;
    session.project_loaded = true;

    ContextAcquisitionSignals signals;
    signals.mutate_subkind = MutateSubkind::Connect;
    signals.from_class = "A";
    signals.to_class = "B";

    const ContextAcquisitionPlan plan =
        computeContextAcquisitionPlan(state, session, LLMGuiContextSnapshot{}, signals,
                                      LLMContextAcquisitionMode::Minimal);
    EXPECT_FALSE(plan.inject_link_patterns);
    EXPECT_FALSE(plan.prefetch_snapshot);
}

TEST(LLMContextAcquisitionPolicy, PrefetchDocsMutateInAutoMode)
{
    ConversationState state;
    LLMSessionContext session;
    session.project_loaded = true;

    ContextAcquisitionSignals signals;
    signals.intent = LLMIntentKind::Mutate;
    signals.mutate_subkind = MutateSubkind::Connect;
    signals.retrieval_query = "connect neurons";

    const ContextAcquisitionPlan auto_plan =
        computeContextAcquisitionPlan(state, session, LLMGuiContextSnapshot{}, signals,
                                      LLMContextAcquisitionMode::Auto);
    EXPECT_TRUE(auto_plan.prefetch_docs);

    const ContextAcquisitionPlan minimal_plan =
        computeContextAcquisitionPlan(state, session, LLMGuiContextSnapshot{}, signals,
                                      LLMContextAcquisitionMode::Minimal);
    EXPECT_FALSE(minimal_plan.prefetch_docs);
}

TEST(LLMContextAcquisitionPolicy, PrefetchSnapshotWithDiagramScope)
{
    ConversationState state;
    state.known_facts = {"fact"};
    LLMSessionContext session;
    session.project_loaded = true;
    LLMGuiContextSnapshot gui;
    gui.diagram_scope_long_name = "Hardware/Drill";

    ContextAcquisitionSignals signals;
    const ContextAcquisitionPlan plan =
        computeContextAcquisitionPlan(state, session, gui, signals,
                                      LLMContextAcquisitionMode::Auto);
    EXPECT_TRUE(plan.prefetch_snapshot);
}
