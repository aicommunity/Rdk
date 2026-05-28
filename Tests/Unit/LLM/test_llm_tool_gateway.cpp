#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMToolGateway, PendingConfirmationForWrite)
{
    ULLMToolRegistry registry;
    LLMToolDefinition def;
    def.name = "test_write";
    def.kind = LLMToolKind::Write;
    def.requires_confirmation = true;
    def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["applied"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);

    ToolInvokeRequest req;
    req.tool_name = "test_write";
    req.arguments = nlohmann::json::object();
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    ToolGatewayResult r = gateway.invoke(req);
    EXPECT_TRUE(r.pending_confirmation);
    EXPECT_FALSE(r.confirmation_id.empty());
}

TEST(LLMToolGateway, AutoApplySkipsPendingConfirmation)
{
    ULLMToolRegistry registry;
    LLMToolDefinition def;
    def.name = "test_write";
    def.kind = LLMToolKind::Write;
    def.requires_confirmation = true;
    def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["applied"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);

    ToolInvokeRequest req;
    req.tool_name = "test_write";
    req.arguments = nlohmann::json::object();
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.auto_apply_writes = true;
    const ToolGatewayResult r = gateway.invoke(req);
    EXPECT_FALSE(r.pending_confirmation);
    EXPECT_TRUE(r.ok);
    EXPECT_TRUE(r.result.value("applied", false));
}

TEST(LLMToolGateway, IdempotencyReturnsCachedResult)
{
    ULLMToolRegistry registry;
    int invoke_count = 0;
    LLMToolDefinition def;
    def.name = "test_idempotent_write";
    def.kind = LLMToolKind::Write;
    def.idempotent = true;
    def.requires_confirmation = false;
    def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(def, [&](const nlohmann::json&) {
        ++invoke_count;
        ToolGatewayResult r;
        r.ok = true;
        r.result["invoke_count"] = invoke_count;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);

    ToolInvokeRequest req;
    req.tool_name = "test_idempotent_write";
    req.arguments = nlohmann::json::object();
    req.idempotency_key = "idem-key-1";
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;

    const ToolGatewayResult first = gateway.invoke(req);
    const ToolGatewayResult second = gateway.invoke(req);
    EXPECT_TRUE(first.ok);
    EXPECT_TRUE(second.ok);
    EXPECT_EQ(first.result.value("invoke_count", 0), 1);
    EXPECT_EQ(second.result.value("invoke_count", 0), 1);
    EXPECT_EQ(invoke_count, 1);
}
