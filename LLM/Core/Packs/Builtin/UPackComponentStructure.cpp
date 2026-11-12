#include "UPackComponentStructure.h"

#include <sstream>

#include "../../Domain/URdkDomainAccess.h"
#include "../../LlmModuleInit.h"
#include "../../LlmPublicApi.h"
#include "../../Observability/ULLMAuditLog.h"
#include "../../Observability/ULLMToolTrace.h"
#include "../../Orchestrator/ULLMComponentStructureGoal.h"
#include "../../Orchestrator/ULLMRecordedToolInvoke.h"
#include "../../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

namespace {

bool dendriteGoalReady(const ParsedDendriteStructureGoal& dendrite)
{
    return dendrite.ok && dendrite.dendrite_count > 0
           && static_cast<int>(dendrite.lengths.size()) == dendrite.dendrite_count;
}

ParsedDendriteStructureGoal detectDendriteGoal(const PackTurnSnapshot& snap)
{
    ParsedDendriteStructureGoal dendrite;
    if(!snap.req)
        return dendrite;
    dendrite = parseDendriteStructureGoal(snap.req->user_text);
    if(!dendrite.ok)
        dendrite = parseDendriteStructureGoal(snap.planning_text);
    return dendrite;
}

} // namespace

PackMatch UPackComponentStructure::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!snap.session || !snap.session->llm_write_enabled)
        return m;
    if(!dendriteGoalReady(detectDendriteGoal(snap)))
        return m;
    m.score = 1.0f;
    m.reason = "dendrite_structure";
    return m;
}

PackHintContribution UPackComponentStructure::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown = "## Component structure\n" + componentStructureEphemeralHint();
    h.extra_tool_names = {"set_property", "calculate_component", "describe_class",
                          "get_component_properties", "ask_user"};
    h.act_or_clarify_recovery_tools = {"set_property", "calculate_component", "ask_user"};
    return h;
}

RecordedStrategyResult UPackComponentStructure::tryRecorded(PackTurnSnapshot& snap)
{
    RecordedStrategyResult out;
    if(!snap.req || !snap.state || !snap.session || !snap.registry || !snap.gateway || !snap.store)
        return out;
    if(snap.skip_pre_llm_funnel || !snap.session->llm_write_enabled)
        return out;
    if(!LLMServices::instance().isInitialized() || !LLMServices::instance().domain().application())
        return out;

    ParsedDendriteStructureGoal dendrite = detectDendriteGoal(snap);
    if(!dendriteGoalReady(dendrite))
        return out;

    URdkDomainAccess& domain = LLMServices::instance().domain();
    const int channel = snap.session->active_channel_index;
    std::string long_name;
    auto tryResolve = [&](const std::string& hint) {
        if(hint.empty() || !long_name.empty())
            return;
        std::string resolved;
        if(domain.resolveComponentLongName(hint, channel, resolved).ok() && !resolved.empty())
            long_name = resolved;
    };
    tryResolve(dendrite.component_token);
    tryResolve(snap.req->gui.focused_component_long_name);
    if(long_name.empty() && !snap.state->session_graph.added_long_names.empty())
        long_name = snap.state->session_graph.added_long_names.back();
    if(long_name.empty())
        tryResolve("PNeuron");
    if(long_name.empty())
        return out;

    std::ostringstream vec_ss;
    for(size_t i = 0; i < dendrite.lengths.size(); ++i)
    {
        if(i)
            vec_ss << ' ';
        vec_ss << dendrite.lengths[i];
    }
    const std::string vec_value = vec_ss.str();
    const std::string soma_value = std::to_string(dendrite.dendrite_count);

    RecordedToolInvokeDeps deps{*snap.registry, *snap.gateway, *snap.store, {}, snap.log_reader};
    auto invokeSet = [&](const std::string& prop, const std::string& value,
                         const char* idem) -> ToolGatewayResult {
        nlohmann::json args = {{"long_name", long_name},
                               {"property_name", prop},
                               {"value", value},
                               {"channel_index", channel}};
        RecordedToolInvokeRequest rreq;
        rreq.session_id = snap.req->session_id;
        rreq.trace_id = snap.req->trace_id;
        rreq.tool_name = "set_property";
        rreq.arguments = args;
        rreq.session = *snap.session;
        rreq.session.session_id = snap.req->session_id;
        rreq.user_text_hint = snap.entity_user_text_hint;
        rreq.idempotency_action_id = idem;
        rreq.force_confirmed = true;
        rreq.confirmed = true;
        rreq.skip_preview = true;
        rreq.append_outcome_assistant = false;
        rreq.skip_turn_tool_trace = true;
        RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
        const LLMToolDefinition* def = snap.registry->find("set_property");
        recordTurnToolInvocation(*snap.state, "set_property", args, recorded.gateway, 0,
                                 def ? def->input_schema : nlohmann::json::object());
        return recorded.gateway;
    };

    ToolGatewayResult t1 = invokeSet("StructureBuildMode", "2", "dendrite_fp_mode");
    ToolGatewayResult t2 =
        t1.ok ? invokeSet("NumSomaMembraneParts", soma_value, "dendrite_fp_soma") : t1;
    ToolGatewayResult t3 =
        t2.ok ? invokeSet("NumDendriteMembranePartsVec", vec_value, "dendrite_fp_vec") : t2;
    ToolGatewayResult t4 = t3;
    if(t3.ok)
    {
        nlohmann::json args = {{"long_name", long_name}, {"channel_index", channel}};
        RecordedToolInvokeRequest rreq;
        rreq.session_id = snap.req->session_id;
        rreq.trace_id = snap.req->trace_id;
        rreq.tool_name = "calculate_component";
        rreq.arguments = args;
        rreq.session = *snap.session;
        rreq.session.session_id = snap.req->session_id;
        rreq.user_text_hint = snap.entity_user_text_hint;
        rreq.idempotency_action_id = "dendrite_fp_calc";
        rreq.force_confirmed = true;
        rreq.confirmed = true;
        rreq.skip_preview = true;
        rreq.append_outcome_assistant = false;
        rreq.skip_turn_tool_trace = true;
        RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
        t4 = recorded.gateway;
        const LLMToolDefinition* def = snap.registry->find("calculate_component");
        recordTurnToolInvocation(*snap.state, "calculate_component", args, t4, 0,
                                 def ? def->input_schema : nlohmann::json::object());
    }

    out.handled = true;
    out.response.ok = t4.ok;
    if(t4.ok)
    {
        out.response.text =
            "Configured " + long_name + " dendrites: StructureBuildMode=2, "
            "NumSomaMembraneParts=" + soma_value + ", NumDendriteMembranePartsVec=\""
            + vec_value + "\", then calculate_component.";
    }
    else
    {
        out.response.text =
            !t4.message.empty() ? t4.message : "Dendrite structure FastPath failed";
        out.response.error = t4.message;
    }
    GetAuditLog().append(t4.ok ? "dendrite_structure_fastpath"
                               : "dendrite_structure_fastpath_failed",
                         {{"long_name", long_name},
                          {"dendrite_count", dendrite.dendrite_count},
                          {"ok", t4.ok},
                          {"error_code", t4.error_code},
                          {"via", "capability_pack"}},
                         snap.req->trace_id, snap.req->session_id);
    if(snap.set_phase)
    {
        snap.set_phase(t4.ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed);
        snap.set_phase(LLMWorkflowPhase::Idle);
    }
    assignTurnTerminal(out.response, TurnTerminal::Completed);
    out.response.tool_trace = snap.state->current_turn_tool_trace;
    snap.store->persistToDisk(snap.req->session_id);
    return out;
}

} // namespace RDK::LLM
