#include "ULLMTaskExecutor.h"

#include "ULLMPlanQuantity.h"
#include "ULLMPlanRepeatPolicy.h"

#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Session/ULLMConversationStore.h"
#include "../Session/ULLMSessionGraphMemory.h"
#include "ULLMPlanExecutor.h"
#include "ULLMStepVerifier.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

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
    GetAuditLog().append("task_planned",
                         {{"plan_id", plan.plan_id},
                          {"confidence", plan.confidence},
                          {"step_count", static_cast<int>(plan.steps.size())}},
                         trace_id, session.session_id);

    if(!LLMServices::instance().isInitialized())
    {
        out.ok = false;
        out.goal_satisfied = false;
        out.summary = "Task executor requires initialized LLM services.";
        return out;
    }

    URdkDomainAccess& domain = LLMServices::instance().domain();
    std::unordered_set<int> completed;
    int replan_attempts = 0;
    bool failed = false;
    std::string fail_reason;

    int guard = 0;
    while(guard++ < static_cast<int>(plan.steps.size()) * 4)
    {
        bool progressed = false;
        for(ExecutionPlanStep& step : plan.steps)
        {
            if(step.status == "done")
                continue;

            const bool deps_ok = std::all_of(step.depends_on.begin(), step.depends_on.end(),
                                             [&](int dep_id) { return completed.count(dep_id) > 0; });
            if(!deps_ok)
                continue;

            progressed = true;
            GetAuditLog().append("task_step_started",
                                 {{"plan_id", plan.plan_id},
                                  {"step_id", step.step_id},
                                  {"tool", step.tool_name}},
                                 trace_id, session.session_id);

            const LLMToolDefinition* def = m_registry.find(step.tool_name);
            if(!def)
            {
                step.status = "failed";
                failed = true;
                fail_reason = "Unknown tool in plan: " + step.tool_name;
                break;
            }

            bool step_ok = false;
            std::string step_fail;
            int retries = 0;
            const int repeat_total =
                toolSupportsPlanStepRepeat(step.tool_name) ? std::max(1, step.repeat_count) : 1;
            while(retries <= std::max(0, options.max_step_retries))
            {
                bool repeat_failed = false;
                for(int rep = 0; rep < repeat_total; ++rep)
                {
                    ToolInvokeRequest invoke;
                    invoke.trace_id = trace_id;
                    invoke.tool_name = step.tool_name;
                    invoke.arguments = step.arguments;
                    if(toolSupportsPlanStepRepeat(step.tool_name) && step.tool_name == "add_component"
                       && repeat_total > 1
                       && invoke.arguments.contains("short_name")
                       && invoke.arguments["short_name"].is_string())
                    {
                        const std::string base = invoke.arguments["short_name"].get<std::string>();
                        invoke.arguments["short_name"] = uniqueShortNameForAddRepeat(base, rep);
                    }
                    invoke.session = session;
                    invoke.confirmed = def->requires_confirmation;
                    if(!plan.goal_en.empty())
                        invoke.user_text_hint = plan.goal_en;

                    const ToolGatewayResult tr = m_gateway.invoke(invoke);
                    if(!tr.ok)
                    {
                        step_fail = tr.message.empty() ? tr.error_code : tr.message;
                        repeat_failed = true;
                        break;
                    }
                    step.last_result = tr.result;
                    if(options.conversation_state && tr.ok && tr.result.is_object())
                    {
                        recordWriteToolOutcome(*options.conversation_state, domain, step.tool_name,
                                               tr.result, session.active_channel_index,
                                               &step.arguments);
                    }
                }

                if(repeat_failed)
                {
                    if(retries < options.max_step_retries)
                    {
                        GetAuditLog().append("task_step_retry",
                                             {{"plan_id", plan.plan_id},
                                              {"step_id", step.step_id},
                                              {"retry", retries + 1},
                                              {"reason", "invoke_failed"}},
                                             trace_id, session.session_id);
                        ++retries;
                        continue;
                    }
                    break;
                }

                VerifyResult vr;
                vr.satisfied = true;
                vr.detail = "no_criteria";
                if(step.success)
                    vr = verifySuccessCriteria(*step.success, domain, session.active_channel_index);

                GetAuditLog().append("task_step_verified",
                                     {{"plan_id", plan.plan_id},
                                      {"step_id", step.step_id},
                                      {"repeat_count", repeat_total},
                                      {"satisfied", vr.satisfied},
                                      {"detail", vr.detail}},
                                     trace_id, session.session_id);

                if(vr.satisfied)
                {
                    step_ok = true;
                    break;
                }

                if(retries < options.max_step_retries)
                {
                    GetAuditLog().append("task_step_retry",
                                         {{"plan_id", plan.plan_id},
                                          {"step_id", step.step_id},
                                          {"retry", retries + 1},
                                          {"reason", vr.detail}},
                                         trace_id, session.session_id);
                    if(step.tool_name == "add_component" && step.arguments.contains("short_name")
                       && step.arguments["short_name"].is_string())
                    {
                        const std::string base = step.arguments["short_name"].get<std::string>();
                        step.arguments["short_name"] = base + std::to_string(retries + 2);
                    }
                    ++retries;
                    continue;
                }

                if(replan_attempts < std::max(0, options.max_replan_attempts))
                {
                    ++replan_attempts;
                    GetAuditLog().append("task_replan",
                                         {{"plan_id", plan.plan_id},
                                          {"step_id", step.step_id},
                                          {"attempt", replan_attempts},
                                          {"reason", vr.detail}},
                                         trace_id, session.session_id);
                    if(step.tool_name == "connect_components")
                    {
                        step.arguments["from_property"] = "";
                        step.arguments["to_property"] = "";
                        retries = 0;
                        continue;
                    }
                }

                step_fail = vr.detail;
                break;
            }

            if(!step_ok)
            {
                step.status = "failed";
                failed = true;
                fail_reason = "Step " + std::to_string(step.step_id) + " failed: " + step_fail;
                break;
            }

            step.status = "done";
            completed.insert(step.step_id);
            out.completed_step_ids.push_back(step.step_id);
            plan.checkpoint_after_step_id = step.step_id;
        }

        if(failed)
            break;
        if(!progressed)
            break;
    }

    const bool all_done =
        std::all_of(plan.steps.begin(), plan.steps.end(),
                    [](const ExecutionPlanStep& s) { return s.status == "done"; });
    if(!all_done && !failed)
    {
        failed = true;
        fail_reason = "Plan execution stalled due to unmet dependencies.";
    }

    bool goal_ok = !failed;
    if(goal_ok && plan.goal_success)
    {
        VerifyResult vr =
            verifySuccessCriteria(*plan.goal_success, domain, session.active_channel_index);
        goal_ok = vr.satisfied;
        GetAuditLog().append("task_goal_verified",
                             {{"plan_id", plan.plan_id},
                              {"satisfied", goal_ok},
                              {"detail", vr.detail}},
                             trace_id, session.session_id);
        if(!goal_ok)
            fail_reason = "Goal criteria not satisfied: " + vr.detail;
    }

    if((failed || !goal_ok) && options.compensate_on_failure)
    {
        ULLMPlanExecutor compensator(m_registry, m_gateway);
        std::string compensation_note;
        const int applied =
            compensator.compensateCompletedWrites(plan, session, trace_id, compensation_note);
        GetAuditLog().append("workflow_compensation_note",
                             {{"plan_id", plan.plan_id},
                              {"note", compensation_note},
                              {"applied", applied}},
                             trace_id, session.session_id);
    }

    out.goal_satisfied = goal_ok;
    out.ok = !failed && goal_ok;
    std::ostringstream summary;
    summary << "Task plan " << plan.plan_id << ": " << out.completed_step_ids.size()
            << " steps completed.";
    if(!out.ok && !fail_reason.empty())
        summary << " " << fail_reason;
    out.summary = summary.str();
    GetAuditLog().append(out.ok ? "task_completed" : "task_failed",
                         {{"plan_id", plan.plan_id},
                          {"completed_steps", static_cast<int>(out.completed_step_ids.size())}},
                         trace_id, session.session_id);
    return out;
}

} // namespace RDK::LLM
