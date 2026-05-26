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
