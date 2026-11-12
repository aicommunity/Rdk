#include "UPackChannelCalc.h"

#include "../../LlmModuleInit.h"
#include "../../LlmPublicApi.h"
#include "../../Observability/ULLMAuditLog.h"
#include "../../Observability/ULLMToolTrace.h"
#include "../../Orchestrator/ULLMChannelCalcCommand.h"
#include "../../Orchestrator/ULLMRecordedToolInvoke.h"
#include "../../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

PackMatch UPackChannelCalc::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!snap.session || !snap.session->llm_write_enabled)
        return m;
    ChannelCalcAction action = detectChannelCalcAction(snap.req->user_text);
    if(action == ChannelCalcAction::None)
        action = detectChannelCalcAction(snap.planning_text);
    if(action == ChannelCalcAction::None)
        return m;
    m.score = 1.0f;
    m.reason = toolNameForChannelCalcAction(action);
    return m;
}

PackHintContribution UPackChannelCalc::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown =
        "## Channel calculation\n"
        "For start/pause/reset/step calculation phrases, prefer "
        "`start_channel_calculation` / `pause_channel_calculation` / "
        "`reset_channel_calculation` / `step_channel_calculation`.";
    h.extra_tool_names = {"start_channel_calculation", "pause_channel_calculation",
                          "reset_channel_calculation", "step_channel_calculation", "ask_user"};
    h.act_or_clarify_recovery_tools = h.extra_tool_names;
    return h;
}

RecordedStrategyResult UPackChannelCalc::tryRecorded(PackTurnSnapshot& snap)
{
    RecordedStrategyResult out;
    if(!snap.req || !snap.state || !snap.session || !snap.registry || !snap.gateway || !snap.store)
        return out;
    if(snap.skip_pre_llm_funnel || !snap.session->llm_write_enabled)
        return out;
    if(!LLMServices::instance().isInitialized() || !LLMServices::instance().domain().application())
        return out;

    ChannelCalcAction calc_action = detectChannelCalcAction(snap.req->user_text);
    if(calc_action == ChannelCalcAction::None)
        calc_action = detectChannelCalcAction(snap.planning_text);
    if(calc_action == ChannelCalcAction::None)
        return out;

    const char* tool_name = toolNameForChannelCalcAction(calc_action);
    const int channel_index =
        channelIndexForCalcRequest(snap.req->user_text, snap.session->active_channel_index);
    nlohmann::json args = {{"channel_index", channel_index}};

    RecordedToolInvokeDeps deps{*snap.registry, *snap.gateway, *snap.store, {}, snap.log_reader};
    RecordedToolInvokeRequest rreq;
    rreq.session_id = snap.req->session_id;
    rreq.trace_id = snap.req->trace_id;
    rreq.tool_name = tool_name;
    rreq.arguments = args;
    rreq.session = *snap.session;
    rreq.session.session_id = snap.req->session_id;
    rreq.user_text_hint = snap.entity_user_text_hint;
    rreq.idempotency_action_id = "channel_calc_fastpath";
    rreq.force_confirmed = true;
    rreq.confirmed = true;
    rreq.skip_preview = true;
    rreq.append_outcome_assistant = false;
    rreq.skip_turn_tool_trace = true;

    RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
    const ToolGatewayResult& tr = recorded.gateway;
    const LLMToolDefinition* calc_def = snap.registry->find(tool_name);
    recordTurnToolInvocation(*snap.state, tool_name, args, tr, 0,
                             calc_def ? calc_def->input_schema : nlohmann::json::object());

    out.handled = true;
    out.response.ok = tr.ok;
    out.response.text = formatChannelCalcUserMessage(calc_action, tr, channel_index);
    if(!tr.ok && !tr.message.empty())
        out.response.error = tr.message;
    GetAuditLog().append(tr.ok ? "channel_calc_fastpath" : "channel_calc_fastpath_failed",
                         {{"tool_name", tool_name},
                          {"channel_index", channel_index},
                          {"ok", tr.ok},
                          {"error_code", tr.error_code},
                          {"via", "capability_pack"}},
                         snap.req->trace_id, snap.req->session_id);
    if(snap.set_phase)
    {
        snap.set_phase(tr.ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed);
        snap.set_phase(LLMWorkflowPhase::Idle);
    }
    assignTurnTerminal(out.response, TurnTerminal::Completed);
    out.response.tool_trace = snap.state->current_turn_tool_trace;
    snap.store->persistToDisk(snap.req->session_id);
    return out;
}

} // namespace RDK::LLM
