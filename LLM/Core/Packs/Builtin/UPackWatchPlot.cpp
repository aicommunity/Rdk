#include "UPackWatchPlot.h"

#include <sstream>

#include "../../Domain/URdkDomainAccess.h"
#include "../../LlmModuleInit.h"
#include "../../LlmPublicApi.h"
#include "../../Observability/ULLMAuditLog.h"
#include "../../Observability/ULLMToolTrace.h"
#include "../../Orchestrator/ULLMRecordedToolInvoke.h"
#include "../../Orchestrator/ULLMWatchPlotGoal.h"
#include "../../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

namespace {

ParsedWatchPlotGoal detectWatchGoal(const PackTurnSnapshot& snap)
{
    ParsedWatchPlotGoal watch_goal;
    if(!snap.req)
        return watch_goal;
    watch_goal = parseWatchPlotGoal(snap.req->user_text);
    if(!watch_goal.ok)
        watch_goal = parseWatchPlotGoal(snap.planning_text);
    return watch_goal;
}

} // namespace

PackMatch UPackWatchPlot::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!snap.session || !snap.session->llm_write_enabled)
        return m;
    if(!detectWatchGoal(snap).ok)
        return m;
    m.score = 1.0f;
    m.reason = "watch_plot";
    return m;
}

PackHintContribution UPackWatchPlot::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown = "## Watch plot\n" + watchPlotEphemeralHint();
    h.extra_tool_names = {"add_watch_series", "create_watch_mdi", "list_watch_mdi",
                          "focus_watch_mdi", "close_watch_mdi", "find_component",
                          "get_net_snapshot", "ask_user"};
    h.act_or_clarify_recovery_tools = {"add_watch_series", "create_watch_mdi", "find_component",
                                       "ask_user"};
    return h;
}

RecordedStrategyResult UPackWatchPlot::tryRecorded(PackTurnSnapshot& snap)
{
    RecordedStrategyResult out;
    if(!snap.req || !snap.state || !snap.session || !snap.registry || !snap.gateway || !snap.store)
        return out;
    if(snap.skip_pre_llm_funnel || !snap.session->llm_write_enabled)
        return out;
    if(!LLMServices::instance().isInitialized() || !LLMServices::instance().domain().application())
        return out;

    ParsedWatchPlotGoal watch_goal = detectWatchGoal(snap);
    if(!watch_goal.ok)
        return out;

    URdkDomainAccess& domain = LLMServices::instance().domain();
    const int channel = snap.session->active_channel_index;
    std::vector<std::string> resolved_names;
    bool nested_required = !watch_goal.nested_hint.empty();
    bool nested_resolved = false;

    if(nested_required)
    {
        const std::vector<std::string>& anchors =
            !watch_goal.anchor_tokens.empty() ? watch_goal.anchor_tokens
                                             : watch_goal.component_tokens;
        for(const std::string& parent_tok : anchors)
        {
            std::string ln;
            if(domain
                   .resolveNestedWatchTarget(parent_tok, watch_goal.nested_hint, channel, ln)
                   .ok()
               && !ln.empty())
            {
                resolved_names.push_back(ln);
                nested_resolved = true;
            }
        }
        if(!nested_resolved && !snap.req->gui.focused_component_long_name.empty())
        {
            std::string ln;
            if(domain
                   .resolveNestedWatchTarget(snap.req->gui.focused_component_long_name,
                                             watch_goal.nested_hint, channel, ln)
                   .ok()
               && !ln.empty())
            {
                resolved_names.push_back(ln);
                nested_resolved = true;
            }
        }
    }
    else
    {
        for(const std::string& token : watch_goal.component_tokens)
        {
            std::string ln;
            if(domain.resolveComponentLongName(token, channel, ln).ok() && !ln.empty())
                resolved_names.push_back(ln);
        }
        if(resolved_names.empty() && !snap.req->gui.focused_component_long_name.empty())
            resolved_names.push_back(snap.req->gui.focused_component_long_name);
    }

    if(nested_required && !nested_resolved)
        return out; // fall through to ReAct — do not plot parent.Output
    if(resolved_names.empty() && !watch_goal.want_new_mdi)
        return out;

    RecordedToolInvokeDeps deps{*snap.registry, *snap.gateway, *snap.store, {}, snap.log_reader};
    int mdi_id = -1;
    std::string surface = "window";
    if(watch_goal.want_new_mdi)
    {
        nlohmann::json args = {{"grid_rows", 1}, {"grid_cols", 1}};
        RecordedToolInvokeRequest rreq;
        rreq.session_id = snap.req->session_id;
        rreq.trace_id = snap.req->trace_id;
        rreq.tool_name = "create_watch_mdi";
        rreq.arguments = args;
        rreq.session = *snap.session;
        rreq.session.session_id = snap.req->session_id;
        rreq.user_text_hint = snap.entity_user_text_hint;
        rreq.idempotency_action_id = "watch_fp_create_mdi";
        rreq.force_confirmed = true;
        rreq.confirmed = true;
        rreq.skip_preview = true;
        rreq.append_outcome_assistant = false;
        rreq.skip_turn_tool_trace = true;
        RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
        const LLMToolDefinition* def = snap.registry->find("create_watch_mdi");
        recordTurnToolInvocation(*snap.state, "create_watch_mdi", args, recorded.gateway, 0,
                                 def ? def->input_schema : nlohmann::json::object());
        if(recorded.gateway.ok)
        {
            mdi_id = recorded.gateway.result.value("mdi_id", -1);
            surface = "mdi";
        }
    }

    bool all_ok = true;
    std::ostringstream summary;
    if(watch_goal.want_new_mdi && surface == "mdi")
        summary << "Created Watches MDI mdi_id=" << mdi_id << ".";
    else
        summary << "Added watch series:";

    if(resolved_names.empty())
    {
        if(!(watch_goal.want_new_mdi && surface == "mdi"))
        {
            all_ok = false;
            summary.str("");
            summary << "Watch FastPath: no component resolved for series";
        }
    }

    for(size_t i = 0; i < resolved_names.size(); ++i)
    {
        nlohmann::json args = {{"long_name", resolved_names[i]},
                               {"property_name", watch_goal.property_name},
                               {"channel_index", channel},
                               {"surface", surface},
                               {"mdi_id", mdi_id},
                               {"chart_index", 0}};
        RecordedToolInvokeRequest rreq;
        rreq.session_id = snap.req->session_id;
        rreq.trace_id = snap.req->trace_id;
        rreq.tool_name = "add_watch_series";
        rreq.arguments = args;
        rreq.session = *snap.session;
        rreq.session.session_id = snap.req->session_id;
        rreq.user_text_hint = snap.entity_user_text_hint;
        rreq.idempotency_action_id = "watch_fp_add_" + std::to_string(i);
        rreq.force_confirmed = true;
        rreq.confirmed = true;
        rreq.skip_preview = true;
        rreq.append_outcome_assistant = false;
        rreq.skip_turn_tool_trace = true;
        RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
        const LLMToolDefinition* def = snap.registry->find("add_watch_series");
        recordTurnToolInvocation(*snap.state, "add_watch_series", args, recorded.gateway, 0,
                                 def ? def->input_schema : nlohmann::json::object());
        if(!recorded.gateway.ok)
        {
            all_ok = false;
            summary.str("");
            summary << (!recorded.gateway.message.empty() ? recorded.gateway.message
                                                          : "add_watch_series failed");
            break;
        }
        if(i == 0 && watch_goal.want_new_mdi)
            summary << " Added:";
        summary << " " << resolved_names[i] << "." << watch_goal.property_name;
    }

    out.handled = true;
    out.response.ok = all_ok;
    out.response.text = summary.str();
    if(!all_ok)
        out.response.error = out.response.text;
    GetAuditLog().append(all_ok ? "watch_plot_fastpath" : "watch_plot_fastpath_failed",
                         {{"count", static_cast<int>(resolved_names.size())},
                          {"surface", surface},
                          {"ok", all_ok},
                          {"via", "capability_pack"}},
                         snap.req->trace_id, snap.req->session_id);
    if(snap.set_phase)
    {
        snap.set_phase(all_ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed);
        snap.set_phase(LLMWorkflowPhase::Idle);
    }
    assignTurnTerminal(out.response, TurnTerminal::Completed);
    out.response.tool_trace = snap.state->current_turn_tool_trace;
    snap.store->persistToDisk(snap.req->session_id);
    return out;
}

} // namespace RDK::LLM
