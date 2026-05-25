#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"

using namespace RDK::LLM;

TEST(LLMPolicy, DenyWriteForGuestUser)
{
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    ToolInvokeRequest req;
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = -1;
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "RBAC_GUEST_DENIED");
}

TEST(LLMPolicy, DenyWriteWithoutProject)
{
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "add_component";
    ToolInvokeRequest req;
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "PROJECT_NOT_LOADED");
}
