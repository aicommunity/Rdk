#include "ULLMPolicyEngine.h"

#include "../Tools/ULLMToolRegistry.h"
#include "ULLMUserRole.h"

namespace RDK::LLM {

PolicyDecision ULLMPolicyEngine::checkToolInvoke(const ToolInvokeRequest& req,
                                                  const LLMToolDefinition& tool,
                                                  const URdkDomainAccess& domain) const
{
    const LLMUserRole role = resolveUserRole(req.session.user_id);
    DomainSessionInfo info = domain.sessionInfo();
    const bool project_loaded = info.project_loaded || req.session.project_loaded;
    if(tool.kind == LLMToolKind::Write && !project_loaded)
    {
        return {false, "PROJECT_NOT_LOADED", "Project must be loaded for write tools"};
    }
    if(tool.kind == LLMToolKind::Write && !req.session.llm_write_enabled)
    {
        return {false, "WRITE_DISABLED", "LLM write operations disabled in settings"};
    }
    if(tool.kind == LLMToolKind::Write && role == LLMUserRole::Guest)
    {
        return {false, "RBAC_GUEST_DENIED",
                "Write tools require operator or admin (GetUserId() >= 0)"};
    }
    if(tool.requires_confirmation && !req.confirmed)
    {
        return {true, "", ""};
    }
    return {true, "", ""};
}

PolicyDecision ULLMPolicyEngine::checkPlan(const ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                           const ULLMToolRegistry& registry) const
{
    if(plan.steps.empty())
        return {false, "PLAN_EMPTY", "Execution plan has no steps"};
    if(static_cast<int>(plan.steps.size()) > 12)
        return {false, "PLAN_TOO_LARGE", "Execution plan exceeds 12 steps"};

    const LLMUserRole role = resolveUserRole(session.user_id);
    for(const ExecutionPlanStep& step : plan.steps)
    {
        const LLMToolDefinition* def = registry.find(step.tool_name);
        if(!def)
            return {false, "PLAN_UNKNOWN_TOOL", "Unknown tool in plan: " + step.tool_name};
        if(def->kind == LLMToolKind::Write && role == LLMUserRole::Guest)
            return {false, "PLAN_RBAC_DENIED", "Plan contains write tools for guest user"};
        if(def->kind == LLMToolKind::Write && !session.llm_write_enabled)
            return {false, "PLAN_WRITE_DISABLED", "Plan contains write tools but LLM write is disabled"};
    }
    return {true, "", ""};
}

} // namespace RDK::LLM
