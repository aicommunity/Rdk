#include "UPackAddComponentDirect.h"

#include <algorithm>

#include "../../Domain/ULLMWriteArgumentNormalizer.h"
#include "../../Domain/URdkDomainAccess.h"
#include "../../LlmModuleInit.h"
#include "../../LlmPublicApi.h"
#include "../../Observability/ULLMAuditLog.h"
#include "../../Orchestrator/ULLMConnectPlanParsing.h"
#include "../../Orchestrator/ULLMPlanQuantity.h"
#include "../../Orchestrator/ULLMRecordedToolInvoke.h"
#include "../../Orchestrator/ULLMWriteToolUserMessage.h"
#include "../../Session/ULLMGuiTurnPin.h"
#include "../../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

namespace {

LLMGuiContextSnapshot guiForAdd(const PackTurnSnapshot& snap)
{
    if(snap.state)
    {
        if(const LLMGuiContextSnapshot* pin = guiContextForWrite(*snap.state))
            return *pin;
    }
    if(snap.req)
        return snap.req->gui;
    return {};
}

nlohmann::json addArgsForRepeat(const nlohmann::json& base_args, int repeat_index)
{
    nlohmann::json args = base_args;
    if(args.contains("short_name") && args["short_name"].is_string())
    {
        const std::string short_base = args["short_name"].get<std::string>();
        args["short_name"] = uniqueShortNameForAddRepeat(short_base, repeat_index);
    }
    return args;
}

bool canAttemptAdd(const PackTurnSnapshot& snap)
{
    if(!snap.req || !snap.state || !snap.session || snap.skip_pre_llm_funnel)
        return false;
    if(!snap.session->llm_write_enabled)
        return false;
    if(snap.state->pending_tool_arguments)
        return false;
    if(isConnectGoalText(snap.planning_text) || isValidateConfigurationGoalText(snap.planning_text))
        return false;
    if(!LLMServices::instance().isInitialized() || !LLMServices::instance().domain().application())
        return false;
    return true;
}

std::optional<PreparedAddComponentInvoke> prepareAdd(const PackTurnSnapshot& snap)
{
    if(!canAttemptAdd(snap))
        return std::nullopt;
    URdkDomainAccess& domain = LLMServices::instance().domain();
    const int qty = snap.state->last_quantity.valid
                        ? std::max(1, std::min(snap.state->last_quantity.primary, 32))
                        : 1;
    return tryPrepareAddComponentDirect(snap.entity_user_text_hint, guiForAdd(snap), domain,
                                        snap.session->active_channel_index, qty,
                                        &snap.state->session_graph);
}

} // namespace

PackMatch UPackAddComponentDirect::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    const auto prep = prepareAdd(snap);
    if(!prep)
        return m;
    m.score = 1.0f;
    m.reason = prep->needs_clarification ? "add_component_clarify" : "add_component";
    return m;
}

PackHintContribution UPackAddComponentDirect::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown =
        "## Add component\n"
        "When the user names a registered class (or continuity cue with last_add), prefer "
        "`add_component` with class_name / parent_long_name / short_name. Do not invent "
        "classes; use list_registered_classes / describe_class if unsure.";
    h.extra_tool_names = {"add_component", "list_registered_classes", "describe_class",
                          "get_net_snapshot", "ask_user"};
    h.act_or_clarify_recovery_tools = {"add_component", "list_registered_classes", "ask_user"};
    return h;
}

RecordedStrategyResult UPackAddComponentDirect::tryRecorded(PackTurnSnapshot& snap)
{
    RecordedStrategyResult out;
    const auto add_prep = prepareAdd(snap);
    if(!add_prep)
        return out;

    if(add_prep->needs_clarification)
    {
        if(!snap.route_clarification)
            return out;
        LLMToolCall call;
        call.name = "add_component";
        call.arguments = add_prep->arguments;
        const nlohmann::json& disambiguation = add_prep->clarification;
        if(disambiguation.is_object() && disambiguation.value("kind", "") == "component")
        {
            out.response = snap.route_clarification(
                call, "component", disambiguation.value("field", "long_name"), disambiguation);
        }
        else
        {
            out.response = snap.route_clarification(
                call, "class", "class_name",
                disambiguation.is_object() ? disambiguation : nlohmann::json::object());
        }
        out.handled = true;
        GetAuditLog().append("add_component_direct_clarify",
                             {{"via", "capability_pack"}}, snap.req->trace_id,
                             snap.req->session_id);
        return out;
    }

    auto invokeOne = [&](const nlohmann::json& args) -> LLMFinalResponse {
        if(snap.invoke_tool_direct)
            return snap.invoke_tool_direct("add_component", args);
        if(!snap.registry || !snap.gateway || !snap.store || !snap.state)
            return {};
        RecordedToolInvokeDeps deps{*snap.registry, *snap.gateway, *snap.store, {},
                                    snap.log_reader};
        RecordedToolInvokeRequest rreq;
        rreq.session_id = snap.req->session_id;
        rreq.trace_id = snap.req->trace_id;
        rreq.tool_name = "add_component";
        rreq.arguments = args;
        rreq.session = *snap.session;
        rreq.session.session_id = snap.req->session_id;
        rreq.gui = guiForAdd(snap);
        rreq.user_text_hint = snap.entity_user_text_hint;
        rreq.idempotency_action_id = "add_component_direct";
        rreq.force_confirmed = true;
        rreq.confirmed = true;
        rreq.skip_preview = true;
        RecordedToolInvokeResult recorded = recordedToolInvoke(*snap.state, deps, rreq);
        LLMFinalResponse resp;
        resp.ok = recorded.gateway.ok;
        resp.text = recorded.outcome_text.empty()
                        ? formatWriteToolUserMessage("add_component", recorded.gateway)
                        : recorded.outcome_text;
        if(!resp.ok && !recorded.gateway.message.empty())
            resp.error = recorded.gateway.message;
        resp.tool_trace = snap.state->current_turn_tool_trace;
        return resp;
    };

    const int add_count = std::max(1, add_prep->repeat_count);
    if(add_count > 1)
    {
        int added = 0;
        const std::string class_name = add_prep->arguments.value("class_name", "");
        for(int rep = 0; rep < add_count; ++rep)
        {
            LLMFinalResponse one = invokeOne(addArgsForRepeat(add_prep->arguments, rep));
            if(!one.ok)
            {
                out.handled = true;
                out.response = std::move(one);
                return out;
            }
            ++added;
        }
        out.handled = true;
        out.response.ok = true;
        out.response.text = "Added " + std::to_string(added) + " component(s)"
                            + (class_name.empty() ? "." : (": " + class_name));
        if(snap.store)
            snap.store->clearPendingToolArguments(snap.req->session_id);
        if(snap.state)
            out.response.tool_trace = snap.state->current_turn_tool_trace;
        GetAuditLog().append("add_component_direct_batch",
                             {{"count", added}, {"via", "capability_pack"}}, snap.req->trace_id,
                             snap.req->session_id);
        return out;
    }

    out.response = invokeOne(add_prep->arguments);
    out.handled = true;
    if(out.response.ok && !out.response.needs_argument_clarification
       && !out.response.pending_confirmation && snap.store)
        snap.store->clearPendingToolArguments(snap.req->session_id);
    GetAuditLog().append(out.response.ok ? "add_component_direct" : "add_component_direct_failed",
                         {{"via", "capability_pack"}}, snap.req->trace_id, snap.req->session_id);
    return out;
}

} // namespace RDK::LLM
