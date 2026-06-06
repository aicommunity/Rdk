#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>

#include "Domain/URdkDomainAccess.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Observability/ULLMAuditLog.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterApplicationTools.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace fs = std::filesystem;

namespace {

fs::path uniqueTempDir()
{
    const auto stamp =
        std::chrono::steady_clock::now().time_since_epoch().count();
    return fs::temp_directory_path() / ("nmsdk_llm_td041_" + std::to_string(stamp));
}

LLMCompletionResult makeCreateToolCall(const fs::path& ini_path)
{
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "";
    LLMToolCall call;
    call.id = "call_create";
    call.name = "create_configuration";
    call.arguments = nlohmann::json{{"project_ini_path", ini_path.string()}};
    mock.tool_calls.push_back(call);
    return mock;
}

} // namespace

TEST(LLMManualE2e, TD041RejectThenAutoApplyRetry)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");
    ::setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);

    const fs::path work = uniqueTempDir();
    fs::remove_all(work);
    fs::create_directories(work);

    ULLMMockProvider provider;
    provider.enqueue(makeCreateToolCall(work / "cfg1" / "project.ini"));
    provider.enqueue(makeCreateToolCall(work / "cfg2" / "project.ini"));

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, GetAuditLog(), idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    // 1) HITL flow: tool_call returns pending confirmation.
    LLMRequestEnvelope req;
    req.session_id = "td041";
    req.trace_id = "t-td041";
    req.user_text = "создай новый проект";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    req.session.auto_apply_writes = false;

    LLMFinalResponse resp = orch.handleUserMessage(req);
    ASSERT_TRUE(resp.pending_confirmation);
    ASSERT_FALSE(resp.pending_confirmation_id.empty());

    // 2) Reject clears pending state.
    orch.rejectPending(req.session_id);

    // 3) Auto-apply: rerun the same user intent, and the gateway executes immediately.
    LLMRequestEnvelope req2 = req;
    req2.trace_id = "t-td041-2";
    req2.session.auto_apply_writes = true;

    const LLMFinalResponse resp2 = orch.handleUserMessage(req2);
    EXPECT_TRUE(resp2.ok) << resp2.error;
    EXPECT_FALSE(resp2.pending_confirmation);

    fs::remove_all(work);
    ::unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}
