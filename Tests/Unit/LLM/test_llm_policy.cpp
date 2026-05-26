#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"

using namespace RDK::LLM;

TEST(LLMPolicy, AllowGraphWriteForGuestWhenWriteEnabled)
{
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "add_component";
    tool.requires_project_loaded = true;
    ToolInvokeRequest req;
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = -1;
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMPolicy, DenyWriteWithoutProject)
{
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "add_component";
    tool.requires_project_loaded = true;
    ToolInvokeRequest req;
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "PROJECT_NOT_LOADED");
}

TEST(LLMPolicy, AllowGuestCreateConfiguration)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "create_configuration";
    tool.requires_project_loaded = false;
    tool.requires_confirmation = true;
    ToolInvokeRequest req;
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    req.session.user_id = -1;
    req.confirmed = true;
    req.arguments = {{"autocreate_subdirectory", true}};
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_TRUE(d.allowed);
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMPolicy, AllowCreateConfigurationAutocreateWithoutParent)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "create_configuration";
    tool.requires_project_loaded = false;
    tool.requires_confirmation = true;
    ToolInvokeRequest req;
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    req.confirmed = true;
    req.arguments = {{"autocreate_subdirectory", true}};
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_TRUE(d.allowed);
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMPolicy, AllowCreateConfigurationWithoutProject)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "create_configuration";
    tool.requires_project_loaded = false;
    tool.requires_confirmation = true;
    ToolInvokeRequest req;
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;
    req.confirmed = true;
    req.arguments = {{"parent_directory", "/tmp"}, {"autocreate_subdirectory", false}};
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_TRUE(d.allowed);
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMPolicy, DenySaveWhenAllowSaveFalse)
{
    ULLMPolicyEngine policy;
    LLMToolDefinition tool;
    tool.kind = LLMToolKind::Write;
    tool.name = "save_configuration";
    tool.requires_project_loaded = true;
    ToolInvokeRequest req;
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.allow_save = false;
    req.confirmed = true;
    URdkDomainAccess domain(nullptr);
    PolicyDecision d = policy.checkToolInvoke(req, tool, domain);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "SAVE_DISABLED");
}
