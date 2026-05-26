#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>

#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterApplicationTools.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(LLMOrchestratorLifecycleArgs, CreateNewConfigUsesLlmToolCall)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "";
    LLMToolCall call;
    call.id = "call_create";
    call.name = "create_configuration";
    const std::string parent =
        (std::filesystem::temp_directory_path() / "nmsdk_llm_orchestrator_create").string();
    std::filesystem::create_directories(parent);
    call.arguments = nlohmann::json{{"project_ini_path", parent + "/project.ini"}};
    mock.tool_calls.push_back(call);
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "lifecycle-create";
    req.trace_id = "t-create";
    req.user_text = "create new config";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_FALSE(resp.needs_argument_clarification);
    EXPECT_FALSE(resp.text.empty());
    EXPECT_EQ(resp.text.find("Mock provider response"), std::string::npos);
}

TEST(LLMOrchestratorLifecycleArgs, LoadConfigWithoutPathUsesLlmToolCall)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "";
    LLMToolCall call;
    call.id = "call_load";
    call.name = "load_configuration";
    call.arguments = nlohmann::json::object();
    mock.tool_calls.push_back(call);
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "lifecycle-args";
    req.trace_id = "t-load";
    req.user_text = "load config";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_TRUE(resp.needs_argument_clarification);
    EXPECT_NE(resp.text.find("load_configuration"), std::string::npos);
}
