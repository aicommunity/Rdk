#include <gtest/gtest.h>

#include <cstdlib>

#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(AskUserProtocol, GatewayDisambiguationUsesAskUserInLoop)
{
    setenv("NMSDK_LLM_CLARIFY_IN_LOOP", "1", 1);
    unsetenv("NMSDK_LLM_INTENT_LLM");
    unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");

    ULLMMockProvider provider;
    LLMCompletionResult first;
    first.ok = true;
    first.tool_calls = {{"tc1", "add_component", {{"class_name", "Foo"}}}};
    provider.enqueue(first);
    provider.enqueue(first);
    provider.enqueue(first);

    ULLMToolRegistry registry;
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    RegisterCoreRdkTools(registry, domain, nullptr);
    RegisterAgentTools(registry);

    LLMToolDefinition add_def;
    add_def.name = "add_component";
    add_def.kind = LLMToolKind::Write;
    add_def.input_schema = {{"type", "object"}};
    registry.registerTool(add_def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = false;
        r.result = {{"ambiguous", true},
                    {"kind", "class"},
                    {"candidates", nlohmann::json::array({{{"class_name", "A"}}, {{"class_name", "B"}}})}};
        return r;
    });

    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "ask-user";
    req.trace_id = "t1";
    req.user_text = "add component Foo";
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_TRUE(resp.awaiting_user_input);
    EXPECT_FALSE(resp.text.empty());
    unsetenv("NMSDK_LLM_CLARIFY_IN_LOOP");
}
