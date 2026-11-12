#include <gtest/gtest.h>

#include "Orchestrator/ULLMCapabilityRiskGate.h"

using namespace RDK::LLM;

TEST(LLMCapabilityRiskGate, DetectsWeakLiteAndSmallParamTags)
{
    EXPECT_TRUE(isWeakCortexModel("ollama-lite", "qwen2.5:7b"));
    EXPECT_TRUE(isWeakCortexModel("custom", "phi3:3b"));
    EXPECT_TRUE(isWeakCortexModel("x", "tinyllama"));
    EXPECT_FALSE(isWeakCortexModel("ollama-thinking", "qwen3:14b"));
    EXPECT_FALSE(isWeakCortexModel("ollama-local", "qwen2.5:14b"));
}

TEST(LLMCapabilityRiskGate, ComplexGoalForMutateWriteAndDescription)
{
    EXPECT_TRUE(isComplexCapabilityGoal("запиши описание проекта", LLMIntentKind::Mutate, true,
                                        true));
    EXPECT_TRUE(
        isComplexCapabilityGoal("add NSPNeuron", LLMIntentKind::Mutate, true, false));
    EXPECT_FALSE(
        isComplexCapabilityGoal("что на схеме", LLMIntentKind::Query, false, false));
}

TEST(LLMCapabilityRiskGate, ScoreWeakComplexCrossesDefaultTau)
{
    CapabilityRiskSignals s;
    s.weak_cortex_profile = true;
    s.complex_goal = true;
    EXPECT_GE(scoreCapabilityRisk(s), capabilityRiskTau());
}

TEST(LLMCapabilityRiskGate, DecideEscalateWhenStrongerAvailable)
{
    CapabilityRiskSignals s;
    s.weak_cortex_profile = true;
    s.complex_goal = true;
    s.stronger_model_available = true;
    s.cascade_already_used = false;
    const CapabilityRiskDecision d = decideCapabilityRisk(s);
    ASSERT_NE(d.action, CapabilityRiskAction::Continue);
    if(!capabilityRiskGateDisabled())
    {
        EXPECT_EQ(d.action, CapabilityRiskAction::Escalate);
        EXPECT_EQ(d.reason_code, "escalate_stronger_model");
    }
}

TEST(LLMCapabilityRiskGate, DecideAbstainWhenNoStrongerOrCascadeUsed)
{
    CapabilityRiskSignals s;
    s.weak_cortex_profile = true;
    s.complex_goal = true;
    s.aoc_exhausted = true;
    s.stronger_model_available = false;
    s.cascade_already_used = false;
    CapabilityRiskDecision d = decideCapabilityRisk(s);
    if(!capabilityRiskGateDisabled())
    {
        EXPECT_EQ(d.action, CapabilityRiskAction::Abstain);
        EXPECT_EQ(d.reason_code, "abstain_no_stronger");
    }

    s.stronger_model_available = true;
    s.cascade_already_used = true;
    d = decideCapabilityRisk(s);
    if(!capabilityRiskGateDisabled())
    {
        EXPECT_EQ(d.action, CapabilityRiskAction::Abstain);
        EXPECT_EQ(d.reason_code, "abstain_after_cascade");
    }
}

TEST(LLMCapabilityRiskGate, AccumulateToolFailures)
{
    TurnToolInvocationView ok;
    ok.tool_name = "get_net_snapshot";
    ok.ok = true;

    TurnToolInvocationView bad;
    bad.tool_name = "inspect_configuration";
    bad.ok = false;
    bad.error_code = "PATH_NOT_ALLOWED";

    TurnToolInvocationView schema;
    schema.tool_name = "update_configuration";
    schema.ok = false;
    schema.error_code = "VALIDATION_ERROR";

    int errs = 0;
    int pathish = 0;
    accumulateToolFailureSignals({ok, bad, schema}, errs, pathish);
    EXPECT_EQ(errs, 2);
    EXPECT_EQ(pathish, 2);
}

TEST(LLMCapabilityRiskGate, FindStrongerFromWeakOrQwen25)
{
    const auto from7b = findStrongerCortexModel("qwen2.5:7b");
    ASSERT_TRUE(from7b.has_value());
    EXPECT_FALSE(from7b->empty());

    const auto from25 = findStrongerCortexModel("qwen2.5:14b");
    ASSERT_TRUE(from25.has_value());

    // Same as thinking default → no escalate target.
    if(from7b)
    {
        const auto same = findStrongerCortexModel(*from7b);
        EXPECT_FALSE(same.has_value());
    }
}
