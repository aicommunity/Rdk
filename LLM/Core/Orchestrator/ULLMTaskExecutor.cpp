#include "ULLMTaskExecutor.h"

#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "ULLMPlanExecutor.h"
#include "ULLMStepVerifier.h"

namespace RDK::LLM {

ULLMTaskExecutor::ULLMTaskExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway)
    : m_registry(registry)
    , m_gateway(gateway)
{
}

TaskExecuteResult ULLMTaskExecutor::execute(ULLMExecutionPlan& plan,
                                            const LLMSessionContext& session,
                                            const std::string& trace_id,
                                            TaskExecuteOptions options)
{
    TaskExecuteResult out;
    // Phase PR5: start with deterministic execute+verify (bounded retries).
    // For now we reuse the existing plan executor to preserve dependency handling.
    ULLMPlanExecutor executor(m_registry, m_gateway);
    const PlanExecutionResult base =
        executor.execute(plan, session, trace_id, planExecuteWithCheckpointOnFailure());

    out.completed_step_ids = base.completed_step_ids;

    bool goal_ok = base.ok;
    if(goal_ok && plan.goal_success && LLMServices::instance().isInitialized())
    {
        VerifyResult vr = verifySuccessCriteria(*plan.goal_success,
                                                LLMServices::instance().domain(),
                                                session.active_channel_index);
        goal_ok = vr.satisfied;
        GetAuditLog().append("task_goal_verified",
                             {{"plan_id", plan.plan_id},
                              {"satisfied", goal_ok},
                              {"detail", vr.detail}},
                             trace_id, session.session_id);
    }

    out.goal_satisfied = goal_ok;
    out.ok = base.ok && goal_ok;
    out.summary = out.ok ? base.summary : ("Task failed verification. " + base.summary);
    GetAuditLog().append(out.ok ? "task_completed" : "task_failed",
                         {{"plan_id", plan.plan_id},
                          {"completed_steps", static_cast<int>(out.completed_step_ids.size())}},
                         trace_id, session.session_id);
    return out;
}

} // namespace RDK::LLM
