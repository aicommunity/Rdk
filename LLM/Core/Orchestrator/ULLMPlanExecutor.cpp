#include "ULLMPlanExecutor.h"

#include "../LlmModuleInit.h"

#include <algorithm>
#include <sstream>

namespace RDK::LLM {

namespace {

struct CompletedWriteRecord {
    int step_id = 0;
    std::string tool_name;
    nlohmann::json result;
};

void collectWriteRecordsFromPlan(const ULLMExecutionPlan& plan, std::vector<CompletedWriteRecord>& out)
{
    for(const ExecutionPlanStep& step : plan.steps)
    {
        if(step.status != "done" || step.last_result.empty())
            continue;
        CompletedWriteRecord rec;
        rec.step_id = step.step_id;
        rec.tool_name = step.tool_name;
        rec.result = step.last_result;
        out.push_back(std::move(rec));
    }
}

} // namespace

ULLMPlanExecutor::ULLMPlanExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway)
    : m_registry(registry)
    , m_gateway(gateway)
{
}

int ULLMPlanExecutor::compensateCompletedWrites(const ULLMExecutionPlan& plan,
                                              const LLMSessionContext& session,
                                              const std::string& trace_id, std::string& note_out)
{
    std::vector<CompletedWriteRecord> completed_writes;
    collectWriteRecordsFromPlan(plan, completed_writes);
    int applied = 0;
    std::ostringstream oss;

    for(auto it = completed_writes.rbegin(); it != completed_writes.rend(); ++it)
    {
        ToolInvokeRequest undo;
        undo.trace_id = trace_id;
        undo.session = session;
        undo.confirmed = true;

        if(it->tool_name == "add_component")
        {
            const std::string long_name = it->result.value("long_name", "");
            if(long_name.empty())
                continue;
            undo.tool_name = "remove_component";
            undo.arguments = {{"long_name", long_name}};
        }
        else if(it->tool_name == "set_property")
        {
            if(!it->result.value("had_previous", false))
                continue;
            undo.tool_name = "set_property";
            undo.arguments = {{"long_name", it->result.value("long_name", "")},
                              {"property_name", it->result.value("property_name", "")},
                              {"value", it->result.value("previous_value", "")}};
            if(undo.arguments["long_name"].get<std::string>().empty())
                continue;
        }
        else
        {
            continue;
        }

        const ToolGatewayResult undo_tr = m_gateway.invoke(undo);
        if(undo_tr.ok)
        {
            ++applied;
            GetAuditLog().append("plan_compensation_applied",
                                 {{"plan_id", plan.plan_id},
                                  {"step_id", it->step_id},
                                  {"tool", undo.tool_name}},
                                 trace_id, session.session_id);
        }
        else
        {
            GetAuditLog().append("plan_compensation_failed",
                                 {{"plan_id", plan.plan_id},
                                  {"step_id", it->step_id},
                                  {"tool", undo.tool_name},
                                  {"error", undo_tr.message}},
                                 trace_id, session.session_id);
        }
    }

    if(applied > 0)
        oss << "Rollback removed/reverted " << applied << " change(s). ";
    note_out = oss.str();
    return applied;
}

PlanExecutionResult ULLMPlanExecutor::execute(ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                            const std::string& trace_id, PlanExecuteOptions options)
{
    PlanExecutionResult result;
    if(options.resume)
        prepareExecutionPlanForResume(plan);

    std::vector<ExecutionPlanStep*> pending;
    pending.reserve(plan.steps.size());
    for(ExecutionPlanStep& step : plan.steps)
    {
        if(step.status == "done")
            continue;
        pending.push_back(&step);
    }

    int guard = 0;
    while(!pending.empty() && guard++ < static_cast<int>(plan.steps.size()) * 2)
    {
        bool progressed = false;
        for(auto it = pending.begin(); it != pending.end();)
        {
            ExecutionPlanStep& step = **it;
            const bool deps_ok = std::all_of(step.depends_on.begin(), step.depends_on.end(),
                                             [&](int dep_id) {
                                                 for(const ExecutionPlanStep& s : plan.steps)
                                                 {
                                                     if(s.step_id == dep_id)
                                                         return s.status == "done";
                                                 }
                                                 return true;
                                             });
            if(!deps_ok)
            {
                ++it;
                continue;
            }

            const LLMToolDefinition* def = m_registry.find(step.tool_name);
            if(!def)
            {
                step.status = "failed";
                result.failed_step_ids.push_back(step.step_id);
                result.ok = false;
                it = pending.erase(it);
                progressed = true;
                continue;
            }

            ToolInvokeRequest invoke;
            invoke.trace_id = trace_id;
            invoke.tool_name = step.tool_name;
            invoke.arguments = step.arguments;
            invoke.session = session;
            if(def->requires_confirmation)
                invoke.confirmed = true;

            const ToolGatewayResult tr = m_gateway.invoke(invoke);
            if(!tr.ok)
            {
                step.status = "failed";
                step.last_result = tr.result;
                result.failed_step_ids.push_back(step.step_id);
                result.ok = false;
                GetAuditLog().append("plan_step_failed",
                                     {{"plan_id", plan.plan_id},
                                      {"step_id", step.step_id},
                                      {"tool", step.tool_name},
                                      {"error", tr.message}},
                                     trace_id, session.session_id);
                it = pending.erase(it);
                progressed = true;
                break;
            }

            step.status = "done";
            step.last_result = tr.result;
            result.completed_step_ids.push_back(step.step_id);
            plan.checkpoint_after_step_id = step.step_id;
            GetAuditLog().append("plan_checkpoint",
                                 {{"plan_id", plan.plan_id},
                                  {"step_id", step.step_id},
                                  {"tool", step.tool_name}},
                                 trace_id, session.session_id);
            GetAuditLog().append("plan_step_done",
                                 {{"plan_id", plan.plan_id},
                                  {"step_id", step.step_id},
                                  {"tool", step.tool_name}},
                                 trace_id, session.session_id);
            it = pending.erase(it);
            progressed = true;
        }
        if(!progressed)
            break;
    }

    if(!pending.empty())
    {
        result.ok = false;
        for(ExecutionPlanStep* step : pending)
        {
            if(step->status != "failed")
                step->status = "skipped";
            result.failed_step_ids.push_back(step->step_id);
        }
    }

    const bool has_remaining =
        std::any_of(plan.steps.begin(), plan.steps.end(),
                    [](const ExecutionPlanStep& s) { return s.status != "done"; });

    if(!result.ok && has_remaining && !options.compensate_on_failure)
    {
        plan.paused = true;
        result.paused_for_resume = true;
        result.checkpoint_after_step_id = plan.checkpoint_after_step_id;
        GetAuditLog().append("plan_checkpoint_paused",
                             {{"plan_id", plan.plan_id},
                              {"checkpoint_after_step_id", plan.checkpoint_after_step_id},
                              {"failed_steps", result.failed_step_ids}},
                             trace_id, session.session_id);
    }
    else if(!result.ok && options.compensate_on_failure)
    {
        std::string note;
        result.compensation_steps_applied =
            compensateCompletedWrites(plan, session, trace_id, note);
        result.compensation_note = note;
        if(!result.completed_step_ids.empty())
        {
            std::ostringstream oss;
            if(result.compensation_steps_applied > 0)
                oss << result.compensation_note;
            oss << "Completed steps: ";
            for(int id : result.completed_step_ids)
                oss << id << " ";
            if(result.compensation_steps_applied == 0)
                oss << "(manual review may be needed for remaining changes).";
            result.compensation_note = oss.str();
        }
        GetAuditLog().append("workflow_compensation_note",
                             {{"plan_id", plan.plan_id},
                              {"note", result.compensation_note},
                              {"applied", result.compensation_steps_applied}},
                             trace_id, session.session_id);
    }

    if(result.ok)
    {
        plan.paused = false;
        plan.checkpoint_after_step_id = 0;
    }

    std::ostringstream summary;
    summary << "Plan " << plan.plan_id << ": " << result.completed_step_ids.size() << " succeeded, "
            << result.failed_step_ids.size() << " failed.";
    if(result.paused_for_resume)
        summary << " Checkpoint saved — use Resume to continue or Rollback to undo completed steps.";
    if(!result.compensation_note.empty())
        summary << " " << result.compensation_note;
    result.summary = summary.str();
    return result;
}

} // namespace RDK::LLM
