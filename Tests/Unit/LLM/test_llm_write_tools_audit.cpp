#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "LlmModuleInit.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterApplicationTools.h"
#include "Tools/ULLMToolGateway.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

namespace {

std::string readAuditFile(const std::filesystem::path& path)
{
    std::ifstream in(path);
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

} // namespace

TEST(LLMWriteToolsAudit, LlmFirstCreateLogsToolInvokeOnly)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");
    ::unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");
    ::setenv("NMSDK_LLM_INPUT_ENSEMBLE", "0", 1);
    ::setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);

    const std::filesystem::path audit_dir =
        std::filesystem::temp_directory_path() / "nmsdk_llm_audit_write_tools";
    std::filesystem::remove_all(audit_dir);
    std::filesystem::create_directories(audit_dir);
    GetAuditLog().setLogDirectory(audit_dir.string());

    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "";
    LLMToolCall call;
    call.id = "call_audit_create";
    call.name = "create_configuration";
    call.arguments =
        nlohmann::json{{"project_ini_path", (audit_dir / "proj/project.ini").string()}};
    mock.tool_calls.push_back(call);
    provider.enqueue(mock);
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, GetAuditLog(), idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "audit-create";
    req.trace_id = "t-audit";
    req.user_text = "создай новый проект";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    LLMFinalResponse resp = orch.handleUserMessage(req);
    ASSERT_TRUE(resp.pending_confirmation);
    ASSERT_FALSE(resp.pending_confirmation_id.empty());

    const std::filesystem::path audit_file = audit_dir / "audit.jsonl";
    ASSERT_TRUE(std::filesystem::is_regular_file(audit_file));
    const std::string log = readAuditFile(audit_file);

    EXPECT_NE(log.find("confirmation_requested"), std::string::npos);
    EXPECT_NE(log.find("create_configuration"), std::string::npos);
    EXPECT_NE(log.find("intent_classified"), std::string::npos);
    EXPECT_EQ(log.find("lifecycle_preflight"), std::string::npos);
    EXPECT_EQ(log.find("tool_invoke_start"), std::string::npos);

    std::filesystem::remove_all(audit_dir);
    ::unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}
