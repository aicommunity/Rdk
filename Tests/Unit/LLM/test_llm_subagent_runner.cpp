#include <gtest/gtest.h>

#include "Orchestrator/ULLMSubagentRunner.h"
#include "Providers/ULLMMockProvider.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

namespace {

struct SubagentFixture {
    ULLMMockProvider provider;
    ULLMToolRegistry registry;
    URdkDomainAccess domain{nullptr};
    ULLMPolicyEngine policy;
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway{registry, policy, domain, audit, idem, validator};

    SubagentFixture()
    {
        RegisterCoreRdkTools(registry, domain, nullptr);
        registry.registerTool(
            []() {
                LLMToolDefinition d;
                d.name = "search_project_docs";
                d.kind = LLMToolKind::Read;
                d.input_schema = {{"type", "object"}};
                return d;
            }(),
            [](const nlohmann::json&) {
                ToolGatewayResult r;
                r.ok = true;
                r.message = "doc hit";
                return r;
            });
        RegisterAgentTools(registry);
        bindSpawnExploreSubagent(provider, registry, gateway);
    }
};

} // namespace

TEST(SubagentRunner, CompletesReadOnlyRound)
{
    SubagentFixture fx;
    LLMCompletionResult round1;
    round1.ok = true;
    round1.tool_calls = {{"s1", "search_project_docs", {{"query", "UNet"}}}};
    fx.provider.enqueue(round1);
    LLMCompletionResult round2;
    round2.ok = true;
    round2.text = "Found documentation about UNet.";
    fx.provider.enqueue(round2);

    ULLMSubagentRunner runner(fx.provider, fx.registry, fx.gateway);
    SubagentRunRequest req;
    req.task = "Find UNet docs";
    req.profile = "explore";
    LLMSessionContext session;
    const SubagentRunResult res = runner.runExplore(req, "tr", "sess", session);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.summary.empty());
    EXPECT_GE(res.rounds_used, 1);
}

TEST(SubagentRunner, DeniesNestedSpawn)
{
    SubagentFixture fx;
    LLMCompletionResult round1;
    round1.ok = true;
    round1.tool_calls = {{"s1", "spawn_explore_subagent", {{"task", "nested"}}}};
    fx.provider.enqueue(round1);
    LLMCompletionResult round2;
    round2.ok = true;
    round2.text = "Nested spawn was denied; continuing.";
    fx.provider.enqueue(round2);

    ULLMSubagentRunner runner(fx.provider, fx.registry, fx.gateway);
    SubagentRunRequest req;
    req.task = "Explore with nested spawn attempt";
    LLMSessionContext session;
    const SubagentRunResult res = runner.runExplore(req, "tr", "sess-nested", session);
    EXPECT_TRUE(res.ok);
    EXPECT_NE(res.summary.find("Nested spawn was denied"), std::string::npos);
}

TEST(SubagentRunner, GatewayHandlerRunsExplore)
{
    SubagentFixture fx;
    LLMCompletionResult round1;
    round1.ok = true;
    round1.text = "Explore summary via gateway.";
    fx.provider.enqueue(round1);

    ToolInvokeRequest invoke;
    invoke.trace_id = "tr-gw";
    invoke.tool_name = "spawn_explore_subagent";
    invoke.arguments = {{"task", "Quick explore"}, {"profile", "explore"}, {"max_rounds", 2}};
    invoke.session.session_id = "sess-gw";

    const ToolGatewayResult tr = fx.gateway.invoke(invoke);
    EXPECT_TRUE(tr.ok);
    EXPECT_EQ(tr.result.value("summary", ""), "Explore summary via gateway.");
    EXPECT_EQ(tr.result.value("profile", ""), "explore");
}
