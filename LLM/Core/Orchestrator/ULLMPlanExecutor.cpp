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

} // namespace

ULLMPlanExecutor::ULLMPlanExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway)
    : m_registry(registry)
    , m_gateway(gateway)
{
}

PlanExecutionResult ULLMPlanExecutor::execute(ULLMExecutionPlan& plan,
                                              const LLMSessionContext& session,
                                              const std::string& trace_id)
{
    PlanExecutionResult result;
    std::vector<CompletedWriteRecord> completed_writes;
    std::vector<ExecutionPlanStep*> pending;
    pending.reserve(plan.steps.size());
    for(ExecutionPlanStep& step : plan.steps)
        pending.push_back(&step);

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
            result.completed_step_ids.push_back(step.step_id);
            if(def->kind == LLMToolKind::Write)
            {
                CompletedWriteRecord rec;
                rec.step_id = step.step_id;
                rec.tool_name = step.tool_name;
                rec.result = tr.result;
                completed_writes.push_back(std::move(rec));
            }
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
            step->status = "skipped";
            result.failed_step_ids.push_back(step->step_id);
        }
    }

    if(!result.ok && !completed_writes.empty())
    {
        for(auto it = completed_writes.rbegin(); it != completed_writes.rend(); ++it)
        {
            if(it->tool_name != "add_component")
                continue;
            const std::string long_name = it->result.value("long_name", "");
            if(long_name.empty())
                continue;

            ToolInvokeRequest undo;
            undo.trace_id = trace_id;
            undo.tool_name = "remove_component";
            undo.arguments = {{"long_name", long_name}};
            undo.session = session;
            undo.confirmed = true;
            const ToolGatewayResult undo_tr = m_gateway.invoke(undo);
            if(undo_tr.ok)
            {
                ++result.compensation_steps_applied;
                GetAuditLog().append("plan_compensation_applied",
                                     {{"plan_id", plan.plan_id},
                                      {"step_id", it->step_id},
                                      {"long_name", long_name}},
                                     trace_id, session.session_id);
            }
            else
            {
                GetAuditLog().append("plan_compensation_failed",
                                     {{"plan_id", plan.plan_id},
                                      {"step_id", it->step_id},
                                      {"long_name", long_name},
                                      {"error", undo_tr.message}},
                                     trace_id, session.session_id);
            }
        }
    }

    if(!result.completed_step_ids.empty() && !result.ok)
    {
        std::ostringstream oss;
        if(result.compensation_steps_applied > 0)
            oss << "Auto-rollback removed " << result.compensation_steps_applied
                << " component(s) from failed plan. ";
        oss << "Completed steps: ";
        for(int id : result.completed_step_ids)
            oss << id << " ";
        if(result.compensation_steps_applied == 0)
            oss << "(manual review may be needed for remaining changes).";
        result.compensation_note = oss.str();
        GetAuditLog().append("workflow_compensation_note",
                             {{"plan_id", plan.plan_id},
                              {"note", result.compensation_note},
                              {"applied", result.compensation_steps_applied}},
                             trace_id, session.session_id);
    }

    std::ostringstream summary;
    summary << "Plan " << plan.plan_id << ": " << result.completed_step_ids.size() << " succeeded, "
            << result.failed_step_ids.size() << " failed.";
    if(!result.compensation_note.empty())
        summary << " " << result.compensation_note;
    result.summary = summary.str();
    return result;
}

} // namespace RDK::LLM
