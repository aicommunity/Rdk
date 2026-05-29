#include <gtest/gtest.h>

#include "Orchestrator/ULLMSubagentRunner.h"
#include "Providers/ULLMMockProvider.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(SubagentRunner, CompletesReadOnlyRound)
{
    ULLMMockProvider provider;
    LLMCompletionResult round1;
    round1.ok = true;
    round1.tool_calls = {{"s1", "search_project_docs", {{"query", "UNet"}}}};
    provider.enqueue(round1);
    LLMCompletionResult round2;
    round2.ok = true;
    round2.text = "Found documentation about UNet.";
    provider.enqueue(round2);

    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
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

    ULLMPolicyEngine policy;
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);

    ULLMSubagentRunner runner(provider, registry, gateway);
    SubagentRunRequest req;
    req.task = "Find UNet docs";
    LLMSessionContext session;
    const SubagentRunResult res = runner.runExplore(req, "tr", "sess", session);
    EXPECT_TRUE(res.ok);
    EXPECT_FALSE(res.summary.empty());
}
