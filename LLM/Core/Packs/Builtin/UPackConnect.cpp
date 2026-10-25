#include "UPackConnect.h"

#include <cstdlib>
#include <filesystem>

#include "../../Context/ULinkPatternCatalog.h"
#include "../../LlmModuleInit.h"
#include "../../LlmPublicApi.h"
#include "../../Observability/ULLMAuditLog.h"
#include "../../Orchestrator/ULLMConnectPlanParsing.h"
#include "../../Orchestrator/ULLMConnectPlanLlmFallback.h"
#include "../../Orchestrator/ULLMContextKnowledgeBlocks.h"
#include "../../Orchestrator/ULLMPlanConfidence.h"
#include "../../Orchestrator/ULLMPlanQuantity.h"
#include "../../Orchestrator/ULLMTaskExecutor.h"
#include "../../Orchestrator/ULLMTaskPlanner.h"

namespace RDK::LLM {

namespace {

bool planHasConnectStep(const ULLMExecutionPlan& plan)
{
    for(const ExecutionPlanStep& step : plan.steps)
    {
        if(step.tool_name == "connect_components")
            return true;
    }
    return false;
}

TaskPlanResult buildConnectTaskPlan(const PackTurnSnapshot& snap)
{
    TaskPlanRequest task_request;
    task_request.goal_en = snap.planning_text;
    task_request.session = *snap.session;
    task_request.project_loaded = snap.session->project_loaded;
    task_request.resolved_quantity = snap.state->last_quantity;
    task_request.state = snap.state;

    TaskPlanResult task_plan =
        buildTaskPlan(*snap.provider, *snap.registry, LLMServices::instance().domain(), task_request);
    if(task_plan.ok)
        return task_plan;

    const bool env_fallback = []() {
        const char* value = std::getenv("NMSDK_LLM_CONNECT_PLAN_LLM");
        return value && value[0] == '1';
    }();
    const bool runtime_fallback =
        LLMServices::instance().settings().runtime().connect_plan_llm_fallback;
    if(!(env_fallback || runtime_fallback))
        return task_plan;

    ULinkPatternCatalog catalog;
    std::filesystem::path root = std::filesystem::current_path();
    for(int i = 0; i < 8 && root.has_parent_path(); ++i)
    {
        if(std::filesystem::exists(root / "CMakeLists.txt"))
            break;
        root = root.parent_path();
    }
    if(!root.empty())
        catalog.loadFromFile(root / "Bin/LLM/index/link-patterns.json");

    ConnectPlanBuildRequest request{snap.planning_text,
                                    parseConnectGoal(snap.planning_text),
                                    *snap.session,
                                    snap.state,
                                    LLMServices::instance().domain(),
                                    catalog,
                                    1};
    ConnectPlanBuildResult fallback = tryBuildConnectPlanViaLlm(request, *snap.provider);
    if(!fallback.ok)
        return task_plan;

    task_plan.ok = true;
    task_plan.plan.plan_id =
        "task_" + std::to_string(std::hash<std::string>{}(snap.planning_text));
    task_plan.plan.goal_en = snap.planning_text;
    task_plan.plan.requires_user_confirmation = true;
    task_plan.plan.confidence = 0.7f;
    task_plan.plan.steps.clear();
    ExecutionPlanStep snapshot;
    snapshot.step_id = 1;
    snapshot.tool_name = "get_net_snapshot";
    snapshot.arguments = {{"channel_index", snap.session->active_channel_index}};
    snapshot.success = SuccessCriteria{"tool_ok", nlohmann::json::object()};
    task_plan.plan.steps.push_back(snapshot);
    int step_id = 2;
    for(auto& step : fallback.steps)
    {
        step.step_id = step_id++;
        step.depends_on = {1};
        task_plan.plan.steps.push_back(std::move(step));
    }
    task_plan.plan.goal_success = fallback.goal_success;
    GetAuditLog().append("connect_plan_llm_fallback",
                         {{"link_count", static_cast<int>(fallback.steps.size())}},
                         snap.req->trace_id, snap.req->session_id);
    return task_plan;
}

} // namespace

PackMatch UPackConnect::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!isConnectGoalText(snap.req->user_text) && !isConnectGoalText(snap.planning_text))
        return m;
    m.score = 1.0f;
    m.reason = "connect";
    return m;
}

PackHintContribution UPackConnect::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown = buildConnectInspectHintBlock();
    h.extra_tool_names = {"connect_components", "list_model_links", "get_component_ports",
                          "get_net_snapshot", "find_component", "ask_user"};
    h.act_or_clarify_recovery_tools = {"connect_components", "list_model_links",
                                       "get_component_ports", "ask_user"};
    return h;
}

RecordedStrategyResult UPackConnect::tryRecorded(PackTurnSnapshot& snap)
{
    RecordedStrategyResult out;
    if(!snap.req || !snap.state || !snap.session || !snap.registry || !snap.gateway || !snap.provider
       || !snap.store || snap.skip_pre_llm_funnel)
    {
        return out;
    }
    if(!LLMServices::instance().isInitialized() || !LLMServices::instance().domain().application())
        return out;

    const ParsedConnectGoal parsed_goal = parseConnectGoal(snap.planning_text);
    if(!parsed_goal.analogous_ref_token)
        return out;

    TaskPlanResult task_plan = buildConnectTaskPlan(snap);
    if(!task_plan.ok || !planHasConnectStep(task_plan.plan))
        return out;

    const std::optional<int> session_quantity =
        snap.state->last_quantity.valid ? std::optional<int>(snap.state->last_quantity.primary)
                                        : std::nullopt;
    applyGoalQuantityToExecutionPlan(task_plan.plan, session_quantity);
    const PlanConfirmDecision confirmation = decidePlanConfirmation(
        task_plan.plan, snap.session->autonomous_mode, snap.session->auto_apply_writes,
        !task_plan.issues.empty());
    GetAuditLog().append("live_analogous_fastpath",
                         {{"plan_id", task_plan.plan.plan_id},
                          {"step_count", static_cast<int>(task_plan.plan.steps.size())},
                          {"needs_confirm", confirmation.needs_user_confirmation},
                          {"via", "capability_pack"}},
                         snap.req->trace_id, snap.req->session_id);

    out.handled = true;
    if(confirmation.needs_user_confirmation)
    {
        snap.state->pending_plan = task_plan.plan;
        out.response.pending_plan_execution = true;
        out.response.pending_plan_id = task_plan.plan.plan_id;
        out.response.text = formatExecutionPlanPreview(task_plan.plan)
                            + "\n\n[Task plan ready — confirm execution in the assistant panel.]";
        if(snap.set_phase)
            snap.set_phase(LLMWorkflowPhase::AwaitingConfirmation);
        assignTurnTerminal(out.response, TurnTerminal::AwaitingConfirm);
        snap.store->persistToDisk(snap.req->session_id);
        return out;
    }

    if(snap.set_phase)
        snap.set_phase(LLMWorkflowPhase::TaskExecuting);
    ULLMTaskExecutor task_executor(*snap.registry, *snap.gateway);
    TaskExecuteOptions options;
    options.conversation_state = snap.state;
    options.conversation_store = snap.store;
    TaskExecuteResult execution =
        task_executor.execute(task_plan.plan, *snap.session, snap.req->trace_id, options);
    out.response.ok = execution.ok;
    out.response.text = execution.summary;
    if(!execution.ok)
        out.response.error = execution.summary;
    GetAuditLog().append(execution.ok ? "task_completed" : "task_failed",
                         {{"summary", execution.summary},
                          {"live_analogous", true},
                          {"via", "capability_pack"}},
                         snap.req->trace_id, snap.req->session_id);
    if(snap.set_phase)
    {
        snap.set_phase(execution.ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed);
        snap.set_phase(LLMWorkflowPhase::Idle);
    }
    assignTurnTerminal(out.response,
                       execution.ok ? TurnTerminal::TaskFastPathCompleted : TurnTerminal::Completed);
    out.response.tool_trace = snap.state->current_turn_tool_trace;
    snap.store->persistToDisk(snap.req->session_id);
    return out;
}

} // namespace RDK::LLM
