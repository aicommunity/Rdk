#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMConnectPlanParsing.h"
#include "Orchestrator/ULLMTaskPlanner.h"
#include "Orchestrator/ULLMTaskPlanParsing.h"
#include "Providers/ILLMProvider.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace {

class NullProvider : public ILLMProvider {
public:
    LLMProviderKind kind() const override { return LLMProviderKind::EmbeddedLlama; }
    LLMProviderCapabilities capabilities() const override { return {}; }
    LLMCompletionResult chat(const std::vector<LLMMessage>&,
                             const LLMCompletionOptions&) override
    {
        return {};
    }
    void chatStream(const std::vector<LLMMessage>&, const LLMCompletionOptions&,
                    LLMStreamCallback,
                    std::function<void(LLMCompletionResult)>) override
    {
    }
    bool healthCheck(std::string&) override { return false; }
    void cancel() override {}
};

} // namespace

TEST(LLMTaskPlanner, RussianAddTwoNeuronsBuildsAddSteps)
{
    NullProvider provider;
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);

    TaskPlanRequest req;
    req.goal_en = "создай два нейрона NSPNeuron";
    req.session.active_channel_index = 0;

    const TaskPlanResult result = buildTaskPlan(provider, registry, domain, req);
    ASSERT_GE(result.plan.steps.size(), 2u);
    EXPECT_EQ(result.plan.steps[1].tool_name, "add_component");
    EXPECT_EQ(result.plan.steps[1].repeat_count, 2);
    EXPECT_EQ(result.plan.steps[1].arguments.value("class_name", ""), "NSPNeuron");
}

TEST(LLMConnectPlanParsing, QuantityAloneIsNotConnectCountOnly)
{
    const ParsedConnectGoal g = parseConnectGoal("создай два нейрона NSPNeuron");
    EXPECT_EQ(g.kind, ConnectGoalKind::None);
    EXPECT_EQ(g.link_count, 2);
}

TEST(LLMTaskPlanParsing, IsAddComponentGoalRussian)
{
    EXPECT_TRUE(isAddComponentGoal("создай два нейрона NSPNeuron"));
    EXPECT_FALSE(isAddComponentGoal("создай новый проект"));
}
