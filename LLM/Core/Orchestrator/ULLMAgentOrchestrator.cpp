#include "ULLMAgentOrchestrator.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <functional>
#include <future>
#include <iomanip>
#include <sstream>

#include <QByteArray>
#include <QCryptographicHash>

#include "../../../Core/Application/UApplication.h"
#include "../LlmModuleInit.h"
#include "../Session/ULLMConfirmationExpiry.h"
#include "../LlmPublicApi.h"
#include "../Packs/ILLMCapabilityPack.h"
#include "../Policy/ULLMPolicyEngine.h"
#include "../Providers/UOllamaChatTemplate.h"
#include "../Providers/UOllamaModelInfo.h"
#include "../Settings/ULLMProviderAuth.h"
#include "../Settings/ULLMResponseLanguage.h"
#include "../Settings/ULLMUserMessages.h"
#include "../Observability/ULLMSystemLogExcerpt.h"
#include "../Observability/ULLMSystemLogReader.h"
#include "../Observability/ULLMToolTrace.h"
#include "../TrustBoundary/ULLMTrustBoundary.h"
#include "../Intent/ULLMIntentAmbiguityGate.h"
#include "ULLMConfigurationLifecycle.h"
#include "ULLMChannelCalcCommand.h"
#include "ULLMComponentStructureGoal.h"
#include "ULLMWatchPlotGoal.h"
#include "ULLMActOrClarifyGate.h"
#include "ULLMDialogSlotMerge.h"
#include "ULLMLifecycleArgumentGate.h"
#include "../Domain/URdkApplicationCommands.h"
#include "ULLMToolFilterBuilder.h"
#include "ULLMWriteToolUserMessage.h"
#include "ULLMWriteToolExecution.h"
#include "ULLMEmbeddedToolCalls.h"
#include "ULLMExecutionPlan.h"
#include "ULLMTaskPlanner.h"
#include "ULLMAgentManifestBuilder.h"
#include "ULLMContextAssembler.h"
#include "../Context/ULLMLongTermMemoryLoader.h"
#include "../Context/ULinkPatternCatalog.h"
#include "../Context/URdkContextRetriever.h"
#include "../Domain/ULLMResolvedEntityStore.h"
#include "../Session/ULLMContextCompactor.h"
#include "../Session/ULLMWorkingGoals.h"
#include "../Session/ULLMGuiTurnPin.h"
#include "../Session/ULLMSessionGraphMemory.h"
#include "ULLMPlanConfidence.h"
#include "ULLMPlanExecutor.h"
#include "ULLMQueryNormalizer.h"
#include "ULLMTaskExecutor.h"
#include "../Knowledge/ULLMDynamicToolRouter.h"
#include "../Policy/ULLMAutonomousPolicy.h"
#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "ULLMQuantityResolver.h"
#include "ULLMPlanQuantity.h"
#include "ULLMTaskPathRouting.h"
#include "ULLMTaskPathMode.h"
#include "ULLMUnifiedTurnController.h"
#include "ULLMInputUnderstanding.h"
#include "ULLMToolExposurePolicy.h"
#include "ULLMContextAcquisitionPolicy.h"
#include "ULLMContextKnowledgeBlocks.h"
#include "../Context/ULLMIndexCatalogs.h"
#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "ULLMConnectPlanParsing.h"
#include "ULLMConnectPlanLlmFallback.h"
#include "../Tools/ULLMToolArgumentValidator.h"
#include "ULLMModelRouter.h"
#include "ULLMClarificationFormat.h"
#include "ULLMRecordedToolInvoke.h"
#include "ULLMTurnTerminalHelpers.h"
#include "ULLMToolFilterExpand.h"

namespace RDK::LLM {

namespace {

std::string pseudoSha256(const std::string& text)
{
    const QByteArray data(text.data(), static_cast<int>(text.size()));
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
}

std::string collectLogExcerptForTool(ULLMSystemLogReader* reader, int active_channel_index)
{
    if(!reader)
        return {};
    const SystemLogPolicySnapshot pol = reader->policy(active_channel_index);
    return reader->collectDeltaExcerpt(ULLMSystemLogReader::autoInjectMaxLines(),
                                       ULLMSystemLogReader::defaultMinSeverityForPolicy(pol));
}

std::string buildToolMessageContent(const ToolGatewayResult& tr, ULLMSystemLogReader* reader,
                                  int active_channel_index)
{
    const std::string excerpt = collectLogExcerptForTool(reader, active_channel_index);
    return sanitizeUntrustedToolContent(toolJsonWithSystemLogExcerpt(tr, excerpt).dump());
}

void snapshotLastSessionContext(ConversationState& state, const LLMSessionContext& session)
{
    syncSessionGraphOnSessionChange(state, session);
    state.last_session_context = session;
    state.last_session_context->session_id = state.session_id;
}

nlohmann::json addComponentArgsForRepeat(const nlohmann::json& base_args, int repeat_index)
{
    nlohmann::json args = base_args;
    if(args.contains("short_name") && args["short_name"].is_string())
    {
        const std::string short_base = args["short_name"].get<std::string>();
        args["short_name"] = uniqueShortNameForAddRepeat(short_base, repeat_index);
    }
    return args;
}

void appendAgentNote(ConversationState& state, const std::string& line)
{
    constexpr std::size_t kMaxNotes = 4096;
    if(!state.agent_notes.empty())
        state.agent_notes += "\n";
    state.agent_notes += line;
    if(state.agent_notes.size() > kMaxNotes)
        state.agent_notes.resize(kMaxNotes);
}

const char* kSessionBusyError = "Session busy: wait for the current request to finish.";

void syncWorkingGoalEvidenceFromTrace(ConversationState& state)
{
    if(state.working_goals.empty())
        return;
    const std::string& goal_id = state.working_goals.front().id;
    for(const TurnToolInvocationView& inv : state.current_turn_tool_trace)
    {
        if(!inv.ok)
            continue;
        appendWorkingGoalEvidence(state, goal_id, inv.tool_name + (inv.ok ? ":ok" : ":fail"));
    }
    bool any_fail = false;
    bool any_ok = false;
    for(const TurnToolInvocationView& inv : state.current_turn_tool_trace)
    {
        any_ok = any_ok || inv.ok;
        any_fail = any_fail || !inv.ok;
    }
    if(any_ok && !any_fail)
        markWorkingGoalStatus(state, goal_id, WorkingGoalStatus::Done);
    else if(any_fail)
        markWorkingGoalStatus(state, goal_id, WorkingGoalStatus::Blocked);
}

void attachTurnToolTrace(ConversationState& state, LLMFinalResponse& response)
{
    syncWorkingGoalEvidenceFromTrace(state);
    response.tool_trace = state.current_turn_tool_trace;
    response.working_goals = state.working_goals;
}

LLMGuiContextSnapshot guiSnapshotForWrite(const ConversationState& state,
                                          const LLMGuiContextSnapshot& fallback)
{
    if(const LLMGuiContextSnapshot* pin = guiContextForWrite(state))
        return *pin;
    return fallback;
}

std::string combinedUserTextHint(const ConversationState& state, const std::string& planning_text)
{
    if(state.last_user_text_original.empty() || state.last_user_text_original == planning_text)
        return planning_text;
    return state.last_user_text_original + "\n" + planning_text;
}

struct SessionBusyScope {
    ULLMAgentOrchestrator* orch = nullptr;
    std::string session_id;
    bool held = false;

    SessionBusyScope(ULLMAgentOrchestrator& o, const std::string& sid, LLMFinalResponse& final)
        : orch(&o)
        , session_id(sid)
    {
        if(!o.tryAcquireSessionBusy(sid))
        {
            final.ok = false;
            final.error = kSessionBusyError;
            return;
        }
        held = true;
    }

    ~SessionBusyScope()
    {
        if(held && orch)
            orch->releaseSessionBusy(session_id);
    }

    explicit operator bool() const { return held; }
};

bool isPortLikeComponentLongName(const std::string& long_name)
{
    const size_t dot = long_name.rfind('.');
    return dot != std::string::npos && dot > 0 && dot + 1 < long_name.size();
}

std::vector<LLMToolCall> filterValidatedEmbeddedToolCalls(std::vector<LLMToolCall> calls,
                                                          const ULLMToolRegistry& registry,
                                                          const std::string& planning_text,
                                                          const std::string& trace_id,
                                                          const std::string& session_id)
{
    ULLMToolArgumentValidator validator;
    const bool connect_goal = isConnectGoalText(planning_text);
    std::vector<LLMToolCall> out;
    out.reserve(calls.size());
    for(LLMToolCall& call : calls)
    {
        const LLMToolDefinition* def = registry.find(call.name);
        if(!def)
            continue;
        std::string schema_err;
        nlohmann::json normalized = validator.normalizeForSchema(call.arguments, def->input_schema);
        if(!validator.validate(normalized, def->input_schema, schema_err))
        {
            GetAuditLog().append("embedded_tool_rejected",
                                 {{"tool_name", call.name}, {"reason", "schema:" + schema_err}},
                                 trace_id, session_id);
            continue;
        }
        call.arguments = std::move(normalized);
        if(connect_goal && call.name == "get_component_properties")
        {
            const std::string long_name = call.arguments.value("long_name", std::string());
            if(isPortLikeComponentLongName(long_name))
            {
                GetAuditLog().append(
                    "embedded_tool_rejected",
                    {{"tool_name", call.name},
                     {"reason", "WRONG_TOOL_FOR_CONNECT:port_like_long_name"},
                     {"long_name", long_name}},
                    trace_id, session_id);
                continue;
            }
        }
        out.push_back(std::move(call));
    }
    return out;
}

bool shouldPromptForMissingToolArguments(const std::string& tool_name,
                                         const ToolGatewayResult& tr,
                                         const std::vector<ToolArgumentFieldSpec>& missing)
{
    if(!missing.empty())
        return true;
    const bool lifecycle = lifecycleActionFromToolName(tool_name) != ConfigurationLifecycleAction::None;
    if(lifecycle && tr.pending_confirmation)
        return true;
    if(!toolInvokeNeedsArgumentClarification(tool_name, tr))
        return false;
    return lifecycle;
}

std::string stableArgumentsJson(const nlohmann::json& args)
{
    if(!args.is_object() && !args.is_array())
        return args.dump();
    return args.dump();
}

std::string makeIdempotencyKey(const std::string& session_id, const std::string& trace_id,
                               const std::string& tool_name, const nlohmann::json& args,
                               const std::string& action_id = std::string())
{
    return pseudoSha256(session_id + "|" + trace_id + "|" + action_id + "|" + tool_name + "|"
                        + stableArgumentsJson(args));
}

bool isRollbackEligibleWriteRecord(const ExecutionPlanStep& step)
{
    if(step.status != "done" || step.last_result.empty())
        return false;
    if(step.tool_name == "add_component")
        return !step.last_result.value("long_name", "").empty();
    if(step.tool_name == "set_property")
        return step.last_result.value("had_previous", false)
               && !step.last_result.value("long_name", "").empty();
    if(step.tool_name == "connect_components")
        return true;
    return false;
}

int rollbackEligibleWriteCount(const ULLMExecutionPlan& plan)
{
    int count = 0;
    for(const ExecutionPlanStep& step : plan.steps)
    {
        if(isRollbackEligibleWriteRecord(step))
            ++count;
    }
    return count;
}

bool extractAmbiguousFindComponent(const ToolGatewayResult& tr, nlohmann::json& candidates_out)
{
    if(!tr.ok || !tr.result.value("ambiguous", false))
        return false;
    candidates_out = tr.result.value("candidates", nlohmann::json::array());
    return candidates_out.size() > 1;
}

bool clarifyInLoopEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_CLARIFY_IN_LOOP");
    if(!v)
        return true;
    return v[0] != '0' && std::strcmp(v, "false") != 0 && std::strcmp(v, "FALSE") != 0;
}

bool extractToolDisambiguationPayload(const ToolGatewayResult& tr, nlohmann::json& payload_out)
{
    if(!tr.result.is_object() || !tr.result.value("ambiguous", false))
        return false;
    const nlohmann::json candidates = tr.result.value("candidates", nlohmann::json::array());
    if(!candidates.is_array() || candidates.empty())
        return false;
    payload_out = tr.result;
    return true;
}

void capturePendingOpenRecentAfterList(ULLMConversationStore& store, const std::string& session_id,
                                       const std::string& tool_name, const ToolGatewayResult& tr)
{
    if(tool_name != "list_recent_configurations" || !tr.ok)
        return;
    if(const std::optional<PendingToolArguments> pending =
           pendingOpenRecentFromConfigurationList(tr.result))
    {
        store.setPendingToolArguments(session_id, *pending);
        store.persistToDisk(session_id);
    }
}

std::optional<ToolGatewayResult>
tryAutoOpenRecentAfterList(const std::string& user_text, RDK::UApplication* app,
                           ULLMConversationStore& store, const std::string& session_id,
                           ULLMToolGateway& gateway, const ULLMToolRegistry& registry,
                           const LLMSessionContext& session, const std::string& trace_id,
                           const std::string& planning_text)
{
    if(!wantsRecentConfiguration(user_text) || !app)
        return std::nullopt;
    const ConversationState& state = store.getOrCreate(session_id);
    if(!state.pending_tool_arguments
       || state.pending_tool_arguments->tool_name != "open_recent_configuration")
        return std::nullopt;

    PendingToolArguments pending = *state.pending_tool_arguments;
    nlohmann::json merged = mergeArgumentsFromUserText(pending, user_text, app);
    if(!merged.contains("index") && !merged.contains("configuration_path"))
        merged["index"] = 1;
    if(!findMissingArgumentsForTool("open_recent_configuration", merged, app, registry).empty())
        return std::nullopt;

    ToolInvokeRequest invoke;
    invoke.trace_id = trace_id;
    invoke.tool_name = "open_recent_configuration";
    invoke.arguments = std::move(merged);
    invoke.idempotency_key =
        makeIdempotencyKey(session_id, trace_id, invoke.tool_name, invoke.arguments, "auto_open_recent");
    invoke.session = session;
    invoke.user_text_hint = planning_text;
    return gateway.invoke(invoke);
}

} // namespace

ULLMAgentOrchestrator::ULLMAgentOrchestrator(ILLMProvider& provider, ULLMToolRegistry& registry,
                                             ULLMToolGateway& gateway, ULLMConversationStore& store)
    : m_provider(provider)
    , m_registry(registry)
    , m_gateway(gateway)
    , m_store(store)
{
    if(ULLMSystemLogReader::isFeatureEnabled() && LLMServices::instance().isInitialized())
    {
        if(RDK::UApplication* app = LLMServices::instance().domain().application())
            m_system_log_reader = std::make_unique<ULLMSystemLogReader>(app);
    }
}

void ULLMAgentOrchestrator::cancel()
{
    m_cancelled = true;
    m_provider.cancel();
}

void ULLMAgentOrchestrator::cancelSession(const std::string& session_id)
{
    {
        std::lock_guard<std::mutex> lock(m_cancel_mu);
        m_cancelled_sessions.insert(session_id);
    }
    m_provider.cancel();
}

void ULLMAgentOrchestrator::setWorkflowPhase(ConversationState& state, LLMWorkflowPhase phase,
                                             const std::string& trace_id)
{
    if(!workflowTransitionAllowed(state.workflow_phase, phase))
        return;
    const std::string from = workflowPhaseName(state.workflow_phase);
    state.workflow_phase = phase;
    GetAuditLog().append("workflow_transition",
                         {{"from", from}, {"to", workflowPhaseName(phase)}}, trace_id,
                         state.session_id);
}

LLMFinalResponse ULLMAgentOrchestrator::handleUserMessage(const LLMRequestEnvelope& req,
                                                          const LLMStreamHandlers* stream)
{
    return ULLMUnifiedTurnController::handleTurn(*this, req, stream);
}

LLMFinalResponse ULLMAgentOrchestrator::handleUserMessageImpl(const LLMRequestEnvelope& req,
                                                              const LLMStreamHandlers* stream)
{
    m_cancelled = false;
    {
        std::lock_guard<std::mutex> lock(m_cancel_mu);
        m_cancelled_sessions.erase(req.session_id);
    }
    LLMFinalResponse final;
    SessionBusyScope busy(*this, req.session_id, final);
    if(!busy)
        return final;

    ConversationState& state = m_store.getOrCreate(req.session_id);
    state.session_id = req.session_id;
    state.current_turn_tool_trace.clear();
    struct TurnToolTraceAttacher {
        ConversationState& turn_state;
        LLMFinalResponse& response;
        ~TurnToolTraceAttacher()
        {
            syncWorkingGoalEvidenceFromTrace(turn_state);
            response.tool_trace = turn_state.current_turn_tool_trace;
            response.working_goals = turn_state.working_goals;
        }
    } turn_tool_trace_attach{state, final};
    snapshotLastSessionContext(state, req.session);

    beginGuiTurnPin(state, req.gui);
    if(!req.gui.focused_component_long_name.empty() || !req.gui.focused_class_name.empty()
       || !req.gui.project_xml_path.empty() || req.gui.snapshot_fingerprint != 0
       || req.gui.channel_index != 0)
        state.last_gui_context = req.gui;

    if(m_system_log_reader)
    {
        m_system_log_reader->syncPaths();
        m_system_log_reader->mark();
    }

    const int confirmation_ttl = defaultPolicyLimits().confirmation_ttl_seconds;
    if(m_store.expirePendingIfStale(req.session_id, confirmation_ttl))
    {
        GetAuditLog().append("confirmation_expired",
                             {{"reason", "ttl"},
                              {"ttl_seconds", confirmation_ttl}},
                             req.trace_id, req.session_id);
    }

    LLMMessage user_msg;
    user_msg.role = LLMMessage::Role::User;
    user_msg.content = req.user_text;
    m_store.appendMessage(req.session_id, user_msg);

    GetAuditLog().append("user_message", {{"length", req.user_text.size()}}, req.trace_id,
                         req.session_id);
    GetAuditLog().append("unified_turn_started", {{"session_id", req.session_id}}, req.trace_id,
                         req.session_id);

    ensureTurnWorkingGoal(state, req.user_text);

    LLMSessionContext session = req.session;
    bool translate_queries_to_en = LLMServices::instance().isInitialized();
    if(LLMServices::instance().isInitialized())
    {
        const LLMRuntimeProviderSettings& runtime_settings =
            LLMServices::instance().settings().runtime();
        translate_queries_to_en = runtime_settings.translate_queries_to_en;
        session.llm_write_enabled = runtime_settings.llm_write_enabled;
        session.auto_apply_writes =
            runtime_settings.llm_write_enabled && runtime_settings.llm_auto_apply_writes;
        session.autonomous_mode = runtime_settings.autonomous_mode;
        session.autonomous_steps_taken = 0;
        session.allow_cloud_llm = runtime_settings.allow_cloud_providers;
    }
    if(const char* tr_env = std::getenv("NMSDK_LLM_TRANSLATE_QUERIES"))
    {
        translate_queries_to_en =
            tr_env[0] != '0' && std::strcmp(tr_env, "false") != 0 && std::strcmp(tr_env, "FALSE") != 0;
    }
    if(LLMServices::instance().isInitialized())
    {
        ProviderAccessCheck access = LLMServices::instance().checkActiveProviderAccess(session);
        if(!access.allowed)
        {
            final.ok = false;
            final.error = access.deny_message;
            GetAuditLog().append(
                "provider_access_denied",
                {{"code", access.deny_code},
                 {"profile", LLMServices::instance().activeProviderProfile().profile_id}},
                req.trace_id, req.session_id);
            return final;
        }

        GetAuditLog().append(
            "provider_invoke",
            {{"profile_id", LLMServices::instance().activeProviderProfile().profile_id},
             {"api_key_present",
              ULLMProviderAuth::hasApiKey(LLMServices::instance().activeProviderProfile(),
                                          LLMServices::instance().settings().runtime())}},
            req.trace_id, req.session_id);
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Running, req.trace_id);

    const bool skip_pre_llm_funnel =
        (state.pending_tool_arguments || state.pending_user_question)
        && isDisambiguationOnlyFollowUp(req.user_text, state);

    const QueryNormalizeResult qnorm =
        normalizeUserQueryForPlanning(m_provider, req.user_text, translate_queries_to_en);
    const std::string planning_text =
        (!qnorm.text_en.empty() ? qnorm.text_en : req.user_text);
    state.last_user_text_original = req.user_text;
    state.last_user_text_en = planning_text;
    const std::string entity_user_text_hint = combinedUserTextHint(state, planning_text);
    if(qnorm.used_llm_translate)
    {
        GetAuditLog().append(
            "query_translated",
            {{"detected_lang", qnorm.detected_lang},
             {"used_llm_translate", qnorm.used_llm_translate},
             {"normalized_length", static_cast<int>(planning_text.size())},
             {"text_en_sha256", pseudoSha256(planning_text)}},
            req.trace_id, req.session_id);
    }
    else if(!qnorm.ok)
    {
        GetAuditLog().append("query_translate_failed",
                             {{"detected_lang", qnorm.detected_lang},
                              {"used_llm_translate", qnorm.used_llm_translate}},
                             req.trace_id, req.session_id);
    }

    if(!skip_pre_llm_funnel && !isDisambiguationOnlyFollowUp(req.user_text, state))
    {
        QuantityResolveRequest qreq;
        qreq.text_original = req.user_text;
        qreq.text_en = planning_text;
        qreq.allow_llm_fallback = true;
        const QuantityResolveResult qres = resolveUserQuantity(qreq, &m_provider);
        state.last_quantity = qres.quantity;
        if(qres.quantity.valid)
        {
            GetAuditLog().append("quantity_resolved",
                                 {{"primary", qres.quantity.primary},
                                  {"source", quantitySourceName(qres.quantity.source)}},
                                 req.trace_id, req.session_id);
        }
    }

    const ConfigurationLifecycleAction lifecycle_action =
        skip_pre_llm_funnel ? ConfigurationLifecycleAction::None
                            : detectConfigurationLifecycleAction(req.user_text);

    LLMIntentKind intent = LLMIntentKind::Query;
    IntentParseResult intent_result;
    TaskPathDecision task_path_decision;
    if(skip_pre_llm_funnel)
    {
        intent = state.intent_contract_kind;
        intent_result.kind = intent;
        intent_result.confidence = state.intent_contract_confidence;
        intent_result.method = "resume";
    }
    else
    {
        intent_result = m_intent.parseWithOptionalLlm(&m_provider, planning_text);
        intent = intent_result.kind;
        const InputUnderstandingResult understanding = understandUserInput(
            &m_provider, planning_text, intent_result, &req.provider_profile, req.trace_id,
            req.session_id);
        intent = understanding.intent;
        intent_result.confidence = understanding.confidence;
        if(understanding.needs_clarification)
            setWorkflowPhase(state, LLMWorkflowPhase::Understanding, req.trace_id);
        // Channel calc / connect / structure / watch must stay Mutate even if Router labels Query.
        if(isChannelCalcGoalText(planning_text) || isChannelCalcGoalText(req.user_text)
           || isConnectGoalText(planning_text) || isConnectGoalText(req.user_text)
           || isDisconnectGoalText(planning_text)
           || isComponentStructureGoal(planning_text) || isComponentStructureGoal(req.user_text)
           || isWatchPlotGoal(planning_text) || isWatchPlotGoal(req.user_text))
        {
            intent = LLMIntentKind::Mutate;
            intent_result.kind = LLMIntentKind::Mutate;
            intent_result.confidence = std::max(intent_result.confidence, 1.f);
        }
        task_path_decision =
            decideTaskPath(planning_text, intent, session.autonomous_mode, &state);
    }

    // DD-PACK-001: capability pack Recorded strategies (channel_calc, …) before TaskPath.
    std::vector<std::string> matched_pack_ids;
    if(!skip_pre_llm_funnel && LLMServices::instance().isInitialized())
    {
        PackTurnSnapshot snap;
        snap.req = &req;
        snap.state = &state;
        snap.session = &session;
        snap.planning_text = planning_text;
        snap.entity_user_text_hint = entity_user_text_hint;
        snap.skip_pre_llm_funnel = skip_pre_llm_funnel;
        snap.registry = &m_registry;
        snap.gateway = &m_gateway;
        snap.store = &m_store;
        snap.log_reader = m_system_log_reader.get();
        snap.set_phase = [this, &state, &req](LLMWorkflowPhase phase) {
            setWorkflowPhase(state, phase, req.trace_id);
        };
        snap.invoke_tool_direct =
            [this, &session, &entity_user_text_hint, &req](const std::string& tool_name,
                                                           const nlohmann::json& arguments) {
                LLMFinalResponse direct = invokeLifecycleToolDirect(
                    req.session_id, req.trace_id, tool_name, arguments, session,
                    entity_user_text_hint);
                ConversationState& st = m_store.getOrCreate(req.session_id);
                attachTurnToolTrace(st, direct);
                return direct;
            };
        snap.route_clarification =
            [this, &state, &req](const LLMToolCall& call, const std::string& kind,
                                 const std::string& field, const nlohmann::json& disambiguation) {
                const PendingDisambiguationKind dkind =
                    kind == "component" ? PendingDisambiguationKind::Component
                                        : PendingDisambiguationKind::Class;
                return routeClarificationOrDisambiguation(state, req.trace_id, call, dkind, field,
                                                          disambiguation);
            };
        RecordedStrategyResult pack_hit =
            tryRecordedCapabilityPacks(LLMServices::instance().packs(), snap, 0.85f,
                                       &matched_pack_ids);
        if(pack_hit.handled)
            return pack_hit.response;
    }

    // DD-STRUCT-001 / DD-WATCH-001 FastPaths: UPackComponentStructure / UPackWatchPlot via packs.

    const LLMTaskPathMode task_path_mode = resolveTaskPathMode();
    const bool task_path_fast = task_path_mode == LLMTaskPathMode::FastPath;
    if(!skip_pre_llm_funnel && intent == LLMIntentKind::Mutate && task_path_decision.use_task_path
       && lifecycle_action == ConfigurationLifecycleAction::None)
    {
        TaskPlanRequest tp_req;
        tp_req.goal_en = planning_text;
        tp_req.session = session;
        tp_req.project_loaded = session.project_loaded;
        tp_req.resolved_quantity = state.last_quantity;
        tp_req.state = &state;
        TaskPlanResult tp = buildTaskPlan(m_provider, m_registry,
                                          LLMServices::instance().domain(), tp_req);
        if(!tp.ok && isConnectGoalText(planning_text))
        {
            const bool env_fallback = []() {
                const char* v = std::getenv("NMSDK_LLM_CONNECT_PLAN_LLM");
                return v && v[0] == '1';
            }();
            const bool runtime_fallback = LLMServices::instance().isInitialized()
                                          && LLMServices::instance()
                                                 .settings()
                                                 .runtime()
                                                 .connect_plan_llm_fallback;
            if((env_fallback || runtime_fallback) && tp_req.state)
            {
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
                ConnectPlanBuildRequest cr{planning_text,
                                           parseConnectGoal(planning_text),
                                           session,
                                           tp_req.state,
                                           LLMServices::instance().domain(),
                                           catalog,
                                           1};
                ConnectPlanBuildResult fb = tryBuildConnectPlanViaLlm(cr, m_provider);
                if(fb.ok)
                {
                    tp.ok = true;
                    tp.plan.plan_id = "task_" + std::to_string(std::hash<std::string>{}(planning_text));
                    tp.plan.goal_en = planning_text;
                    tp.plan.requires_user_confirmation = true;
                    tp.plan.confidence = 0.7f;
                    tp.plan.steps.clear();
                    ExecutionPlanStep snap;
                    snap.step_id = 1;
                    snap.tool_name = "get_net_snapshot";
                    snap.arguments = {{"channel_index", req.session.active_channel_index}};
                    snap.success = SuccessCriteria{"tool_ok", nlohmann::json::object()};
                    tp.plan.steps.push_back(snap);
                    int sid = 2;
                    for(auto& s : fb.steps)
                    {
                        s.step_id = sid++;
                        s.depends_on = {1};
                        tp.plan.steps.push_back(std::move(s));
                    }
                    tp.plan.goal_success = fb.goal_success;
                    GetAuditLog().append("connect_plan_llm_fallback",
                                         {{"link_count", static_cast<int>(fb.steps.size())}},
                                         req.trace_id, req.session_id);
                }
            }
        }
        if(tp.ok && task_path_fast)
        {
            const std::optional<int> session_qty =
                state.last_quantity.valid
                    ? std::optional<int>(state.last_quantity.primary)
                    : std::nullopt;
            applyGoalQuantityToExecutionPlan(tp.plan, session_qty);
            const PlanConfirmDecision confirm_decision = decidePlanConfirmation(
                tp.plan, session.autonomous_mode, session.auto_apply_writes, !tp.issues.empty());
            if(confirm_decision.needs_user_confirmation)
            {
                state.pending_plan = tp.plan;
                final.pending_plan_execution = true;
                final.pending_plan_id = tp.plan.plan_id;
                final.text = formatExecutionPlanPreview(tp.plan)
                             + "\n\n[Task plan ready — confirm execution in the assistant panel.]";
                setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                assignTurnTerminal(final, TurnTerminal::AwaitingConfirm);
                m_store.persistToDisk(req.session_id);
                return final;
            }

            setWorkflowPhase(state, LLMWorkflowPhase::TaskExecuting, req.trace_id);
            ULLMTaskExecutor task_executor(m_registry, m_gateway);
            TaskExecuteOptions task_opts;
            task_opts.conversation_state = &state;
            task_opts.conversation_store = &m_store;
            TaskExecuteResult exec = task_executor.execute(tp.plan, session, req.trace_id, task_opts);
            final.ok = exec.ok;
            final.text = exec.summary;
            if(!exec.ok)
                final.error = exec.summary;
            GetAuditLog().append(exec.ok ? "task_completed" : "task_failed",
                                 {{"summary", exec.summary}}, req.trace_id, req.session_id);
            if(exec.ok)
                appendAgentNote(state, "Autonomous task completed: " + exec.summary.substr(0, 200));
            setWorkflowPhase(
                state, exec.ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed, req.trace_id);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            assignTurnTerminal(final,
                               exec.ok ? TurnTerminal::TaskFastPathCompleted : TurnTerminal::Completed);
            m_store.persistToDisk(req.session_id);
            return final;
        }
        // DD-ACT / TD-152: live analogous connect plans execute even under HintOnly.
        // TODO(pack-phase-B+): migrate this TaskPlan-coupled execute into UPackConnect::tryRecorded
        // when ConnectPlan build can run without the surrounding task-path ladder.
        const ParsedConnectGoal parsed_live = parseConnectGoal(planning_text);
        bool plan_has_connect = false;
        for(const ExecutionPlanStep& s : tp.plan.steps)
        {
            if(s.tool_name == "connect_components")
            {
                plan_has_connect = true;
                break;
            }
        }
        if(tp.ok && parsed_live.analogous_ref_token && plan_has_connect)
        {
            const std::optional<int> session_qty =
                state.last_quantity.valid
                    ? std::optional<int>(state.last_quantity.primary)
                    : std::nullopt;
            applyGoalQuantityToExecutionPlan(tp.plan, session_qty);
            const PlanConfirmDecision confirm_decision = decidePlanConfirmation(
                tp.plan, session.autonomous_mode, session.auto_apply_writes, !tp.issues.empty());
            GetAuditLog().append("live_analogous_fastpath",
                                 {{"plan_id", tp.plan.plan_id},
                                  {"step_count", static_cast<int>(tp.plan.steps.size())},
                                  {"needs_confirm", confirm_decision.needs_user_confirmation}},
                                 req.trace_id, req.session_id);
            if(confirm_decision.needs_user_confirmation)
            {
                state.pending_plan = tp.plan;
                final.pending_plan_execution = true;
                final.pending_plan_id = tp.plan.plan_id;
                final.text = formatExecutionPlanPreview(tp.plan)
                             + "\n\n[Task plan ready — confirm execution in the assistant panel.]";
                setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                assignTurnTerminal(final, TurnTerminal::AwaitingConfirm);
                m_store.persistToDisk(req.session_id);
                return final;
            }

            setWorkflowPhase(state, LLMWorkflowPhase::TaskExecuting, req.trace_id);
            ULLMTaskExecutor task_executor(m_registry, m_gateway);
            TaskExecuteOptions task_opts;
            task_opts.conversation_state = &state;
            task_opts.conversation_store = &m_store;
            TaskExecuteResult exec = task_executor.execute(tp.plan, session, req.trace_id, task_opts);
            final.ok = exec.ok;
            final.text = exec.summary;
            if(!exec.ok)
                final.error = exec.summary;
            GetAuditLog().append(exec.ok ? "task_completed" : "task_failed",
                                 {{"summary", exec.summary}, {"live_analogous", true}},
                                 req.trace_id, req.session_id);
            if(exec.ok)
                appendAgentNote(state, "Live analogous connect completed: " + exec.summary.substr(0, 200));
            setWorkflowPhase(
                state, exec.ok ? LLMWorkflowPhase::Completed : LLMWorkflowPhase::Failed, req.trace_id);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            assignTurnTerminal(final,
                               exec.ok ? TurnTerminal::TaskFastPathCompleted : TurnTerminal::Completed);
            m_store.persistToDisk(req.session_id);
            return final;
        }
        if(tp.ok)
        {
            appendAgentNote(state,
                            "[Task planner hint]\n" + formatExecutionPlanPreview(tp.plan).substr(0, 1200));
            GetAuditLog().append("task_plan_hint",
                                 {{"plan_id", tp.plan.plan_id},
                                  {"step_count", static_cast<int>(tp.plan.steps.size())}},
                                 req.trace_id, req.session_id);
        }
        else if(task_path_fast)
        {
            final.ok = false;
            final.error = "Task plan could not be built: "
                          + (tp.issues.empty() ? std::string("unknown_issue")
                                               : tp.issues.front());
            final.text = final.error
                         + ". [Enable connect plan LLM fallback or rephrase.]";
            setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            m_store.persistToDisk(req.session_id);
            return final;
        }
        else
        {
            GetAuditLog().append(
                "task_plan_fallback_to_agent",
                {{"issues", tp.issues.empty() ? nlohmann::json::array() : nlohmann::json(tp.issues)}},
                req.trace_id, req.session_id);
        }
    }
    const char* intent_name = "query";
    switch(intent)
    {
    case LLMIntentKind::Mutate:
        intent_name = "mutate";
        break;
    case LLMIntentKind::Explain:
        intent_name = "explain";
        break;
    case LLMIntentKind::Plan:
        intent_name = "plan";
        break;
    default:
        break;
    }
    GetAuditLog().append("intent_classified",
                         {{"kind", intent_name},
                          {"confidence", intent_result.confidence},
                          {"method", intent_result.method}},
                         req.trace_id, req.session_id);
    state.intent_contract_kind = intent;
    state.intent_contract_confidence = intent_result.confidence;
    state.intent_contract_requires_confirmation_for_writes =
        intent != LLMIntentKind::Mutate || session.autonomous_mode == LLMAutonomousMode::Off;
    GetAuditLog().append("intent_contract_set",
                         {{"kind", intent_name},
                          {"confidence", intent_result.confidence},
                          {"requires_confirmation_for_writes",
                           state.intent_contract_requires_confirmation_for_writes}},
                         req.trace_id, req.session_id);
    if(intent == LLMIntentKind::Mutate && !session.llm_write_enabled)
    {
        final.ok = false;
        final.error =
            "LLM write tools are disabled. Enable \"Allow LLM write tools\" in AI Assistant Settings.";
        setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
        m_store.persistToDisk(req.session_id);
        return final;
    }

    if(LLMServices::instance().isInitialized())
    {
        const LLMRuntimeProviderSettings& runtime_settings =
            LLMServices::instance().settings().runtime();
        if(intent == LLMIntentKind::Mutate && runtime_settings.autonomous_mode != LLMAutonomousMode::Off)
        {
            session.autonomous_mode = runtime_settings.autonomous_mode;
            if(session.autonomous_mode == LLMAutonomousMode::SemiAuto)
                session.auto_apply_writes = runtime_settings.llm_write_enabled;
            GetAuditLog().append("autonomous_run_started",
                                 {{"mode", session.autonomous_mode == LLMAutonomousMode::Strict
                                           ? "strict"
                                           : "semi_auto"}},
                                 req.trace_id, req.session_id);
        }
    }

    RDK::UApplication* app = nullptr;
    if(LLMServices::instance().isInitialized())
        app = LLMServices::instance().domain().application();

    if(app)
    {
        if(const std::optional<nlohmann::json> open_recent_args = tryBuildOpenRecentInvokeArguments(
               lifecycle_action, req.user_text, state, app, m_registry))
        {
            LLMFinalResponse direct = invokeLifecycleToolDirect(
                req.session_id, req.trace_id, "open_recent_configuration", *open_recent_args, session,
                entity_user_text_hint);
            if(direct.ok && !direct.needs_argument_clarification && !direct.pending_confirmation)
                m_store.clearPendingToolArguments(req.session_id);
            attachTurnToolTrace(state, direct);
            return direct;
        }
        if(lifecycle_action == ConfigurationLifecycleAction::Load
           && wantsRecentConfiguration(req.user_text))
        {
            const URdkApplicationCommands cmds(app);
            const nlohmann::json list = cmds.listRecentConfigurations();
            const nlohmann::json items = list.value("items", nlohmann::json::array());
            if(!items.is_array() || items.empty())
            {
                LLMFinalResponse response;
                response.ok = false;
                response.text =
                    "No recent configurations are available. Open or create a project first.";
                setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return response;
            }
        }
    }

    // DD-MEM-002/003: add_component direct FastPath → UPackAddComponentDirect (packs).

    if(lifecycleDirectInvokeEnabled() && lifecycle_action == ConfigurationLifecycleAction::Load
       && app && !wantsRecentConfiguration(req.user_text))
    {
        PendingToolArguments bootstrap;
        bootstrap.tool_name = "load_configuration";
        bootstrap.action = ConfigurationLifecycleAction::Load;
        bootstrap.partial_arguments = nlohmann::json::object();
        nlohmann::json merged = mergeArgumentsFromUserText(bootstrap, req.user_text, app);
        const std::vector<ToolArgumentFieldSpec> missing =
            findMissingArgumentsForTool("load_configuration", merged, app, m_registry);
        if(missing.empty() && merged.contains("configuration_path"))
        {
            LLMFinalResponse direct = invokeLifecycleToolDirect(
                req.session_id, req.trace_id, "load_configuration", merged, session,
                entity_user_text_hint);
            if(direct.ok && !direct.needs_argument_clarification && !direct.pending_confirmation)
                m_store.clearPendingToolArguments(req.session_id);
            attachTurnToolTrace(state, direct);
            return direct;
        }
    }

    if(state.pending_user_question && skip_pre_llm_funnel)
    {
        state.known_facts.push_back("User answer: " + req.user_text);
        GetAuditLog().append("ask_user_answered",
                             {{"question_id", state.pending_user_question->question_id}},
                             req.trace_id, req.session_id);
        state.pending_user_question.reset();
        setWorkflowPhase(state, LLMWorkflowPhase::Running, req.trace_id);
    }

    if(intent == LLMIntentKind::Mutate && !state.pending_tool_arguments
       && lifecycle_action == ConfigurationLifecycleAction::None
       && !isValidateConfigurationGoalText(planning_text))
    {
        const DialogSlotMergeInput slot_in{&state.messages, nullptr, &req.gui, &m_registry};
        const DialogSlotMergeResult slot = mergeDialogSlotsIntoToolArgs(slot_in);
        if(slot.has_connect_endpoints || !slot.partial_arguments.empty())
        {
            const bool connect_goal = isConnectGoalText(planning_text);
            const bool disconnect_goal = isDisconnectGoalText(planning_text);
            if(!(connect_goal && disconnect_goal))
            {
                PendingToolArguments pending_slot;
                if(disconnect_goal)
                    pending_slot.tool_name = "disconnect_components";
                else if(!slot.inferred_tool_name.empty())
                    pending_slot.tool_name = slot.inferred_tool_name;
                else
                    pending_slot.tool_name = "connect_components";
                pending_slot.partial_arguments = slot.partial_arguments;
                const std::vector<ToolArgumentFieldSpec> slot_missing =
                    findMissingArgumentsForTool(pending_slot.tool_name, pending_slot.partial_arguments,
                                              app, m_registry);
                if(slot_missing.empty())
                {
                    pending_slot.created_at_unix_sec = confirmationNowUnixSec();
                    m_store.setPendingToolArguments(req.session_id, pending_slot);
                    GetAuditLog().append("dialog_slot_merge_pending",
                                         {{"tool_name", pending_slot.tool_name},
                                          {"has_connect_endpoints", slot.has_connect_endpoints}},
                                         req.trace_id, req.session_id);
                }
            }
        }
    }

    if(state.pending_tool_arguments)
    {
        PendingToolArguments pending = *state.pending_tool_arguments;
        const std::string trimmed_user = req.user_text;
        const bool user_picked_single_class =
            trimmed_user.find_first_of(" \t\n\r") == std::string::npos && !trimmed_user.empty();
        if(pending.tool_name == "add_component"
           && (!user_picked_single_class
               || lifecycle_action != ConfigurationLifecycleAction::None))
            m_store.clearPendingToolArguments(req.session_id);
        else if(lifecycle_action != ConfigurationLifecycleAction::None
                && lifecycle_action != pending.action)
            m_store.clearPendingToolArguments(req.session_id);
        else
        {
            const std::string trimmed_reply = trimmed_user;
            const nlohmann::json candidates =
                pending.disambiguation_candidates.is_array()
                        && !pending.disambiguation_candidates.empty()
                    ? pending.disambiguation_candidates
                    : pending.class_disambiguation_candidates;
            const char* candidate_key =
                pending.disambiguation_kind == PendingDisambiguationKind::Component ? "long_name"
                                                                                     : "class_name";
            if(pending.tool_name != "open_recent_configuration" && candidates.is_array()
               && !candidates.empty() && !trimmed_reply.empty())
            {
                bool all_digits = true;
                for(char c : trimmed_reply)
                {
                    if(!std::isdigit(static_cast<unsigned char>(c)))
                    {
                        all_digits = false;
                        break;
                    }
                }
                if(all_digits && !pickFromNumberedList(trimmed_reply, candidates, candidate_key))
                {
                    LLMFinalResponse response;
                    response.ok = true;
                    response.needs_argument_clarification = true;
                    response.needs_tool_disambiguation = true;
                    response.needs_entity_clarification = true;
                    response.clarification_candidates = candidates;
                    nlohmann::json payload;
                    payload["kind"] =
                        pending.disambiguation_kind == PendingDisambiguationKind::Component
                            ? "component"
                            : "class";
                    payload["candidates"] = candidates;
                    response.text = "Invalid list number. Please choose 1-"
                                   + std::to_string(candidates.size()) + ".\n\n"
                                   + formatClarificationMessage(payload);
                    m_store.setPendingToolArguments(req.session_id, pending);
                    m_store.persistToDisk(req.session_id);
                    return response;
                }
            }

            nlohmann::json merged =
                mergeArgumentsFromUserText(pending, req.user_text, app);
            const std::vector<ToolArgumentFieldSpec> still_missing =
                findMissingArgumentsForTool(pending.tool_name, merged, app, m_registry);
            if(!still_missing.empty())
            {
                pending.partial_arguments = std::move(merged);
                pending.missing_fields = still_missing;
                return returnArgumentRequest(state, req.trace_id, pending, app);
            }
            pending.partial_arguments = merged;
            if(pending.disambiguation_kind == PendingDisambiguationKind::Component
               && merged.contains("long_name") && merged["long_name"].is_string())
            {
                std::string query_text = pending.disambiguation_field;
                if(pending.partial_arguments.contains(pending.disambiguation_field)
                   && pending.partial_arguments[pending.disambiguation_field].is_string())
                    query_text =
                        pending.partial_arguments[pending.disambiguation_field].get<std::string>();
                upsertResolvedEntity(state, "component", query_text,
                                     merged["long_name"].get<std::string>(),
                                     session.active_channel_index);
            }
            m_store.setPendingToolArguments(req.session_id, pending);
            GetAuditLog().append("lifecycle_args_resolved",
                                 {{"tool_name", pending.tool_name}}, req.trace_id,
                                 req.session_id);
            const int add_count =
                pending.tool_name == "add_component"
                    ? std::max(1, pending.requested_repeat_count)
                    : 1;
            if(pending.tool_name == "add_component" && add_count > 1)
            {
                int added = 0;
                std::string class_name = merged.value("class_name", "");
                for(int rep = 0; rep < add_count; ++rep)
                {
                    const nlohmann::json rep_args = addComponentArgsForRepeat(merged, rep);
                    LLMFinalResponse one = invokeLifecycleToolDirect(
                        req.session_id, req.trace_id, pending.tool_name, rep_args, session,
                        entity_user_text_hint);
                    if(!one.ok)
                        return one;
                    ++added;
                    if(!class_name.empty() && rep_args.contains("class_name"))
                        class_name = rep_args["class_name"].get<std::string>();
                }
                LLMFinalResponse response;
                response.ok = true;
                response.text = "Added " + std::to_string(added) + " component(s)"
                             + (class_name.empty() ? "." : (": " + class_name));
                m_store.clearPendingToolArguments(req.session_id);
                return response;
            }

            // Merged args already include the user's pick; avoid re-merging user_text in gateway.
            LLMFinalResponse response = invokeLifecycleToolDirect(
                req.session_id, req.trace_id, pending.tool_name, merged, session,
                entity_user_text_hint);
            if(response.ok && !response.needs_argument_clarification && !response.needs_entity_clarification
               && !response.needs_tool_disambiguation && !response.pending_confirmation)
                m_store.clearPendingToolArguments(req.session_id);
            return response;
        }
    }

    LLMContextAcquisitionMode acquisition_mode = LLMContextAcquisitionMode::Auto;
    if(LLMServices::instance().isInitialized())
        acquisition_mode = LLMServices::instance().settings().runtime().context_acquisition_mode;

    const ContextAcquisitionSignals ctx_signals =
        buildContextAcquisitionSignals(state, session, req.gui, intent, planning_text);
    const ContextAcquisitionPlan ctx_plan = computeContextAcquisitionPlan(
        state, session, req.gui, ctx_signals, acquisition_mode);
    if(ctx_plan.bootstrap_session && !state.session_context_seeded)
        state.session_context_seeded = true;
    const std::string known_facts_block = formatKnownFactsBlock(state.known_facts);
    if(!known_facts_block.empty())
        appendAgentNote(state, known_facts_block);

    ToolFilter filter = buildToolExposureFilter(intent, session.llm_write_enabled, lifecycle_action,
                                                state.intent_contract_confidence);
    filter = ULLMDynamicToolRouter::apply(filter, planning_text);

    bool context_compacted = false;
    {
        ULLMContextCompactor compactor;
        std::string session_storage;
        if(LLMServices::instance().isInitialized() && LLMServices::instance().projectContext())
        {
            session_storage = LLMServices::instance().projectContext()->paths().repository_root.string()
                              + "/LLM/sessions";
        }
        if(compactor.maybeCompact(state, req.session_id, session_storage, &m_provider))
        {
            context_compacted = true;
            m_store.persistToDisk(req.session_id);
        }
    }

    std::vector<LLMMessage> provider_messages;
    const bool strict_plan_schema =
        intent == LLMIntentKind::Plan && providerSupportsStrictPlanSchema(req.provider_profile)
        && m_provider.capabilities().supports_strict_json_schema;
    if(intent == LLMIntentKind::Plan)
        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);

    const bool provider_tools = m_provider.capabilities().supports_tool_calling;

    const ModelRoute cortex_route = routeModelForPhase(ModelTier::Cortex, req.provider_profile);
    applyModelRouteAudit(cortex_route, req.trace_id, req.session_id);

    EphemeralContextInput ctx_input{state,
                                    session,
                                    req.gui,
                                    intent,
                                    lifecycle_action,
                                    provider_tools,
                                    strict_plan_schema,
                                    &m_registry,
                                    filter,
                                    planning_text,
                                    m_system_log_reader
                                        ? m_system_log_reader->policy(session.active_channel_index)
                                              .summary_for_model
                                        : std::string{},
                                    LLMServices::instance().isInitialized()
                                        ? LLMServices::instance().contextRetriever()
                                        : nullptr,
                                    {},
                                    {}};

    if(provider_tools && ctx_plan.prefetch_docs && LLMServices::instance().isInitialized()
       && !ctx_signals.retrieval_query.empty())
    {
        ctx_input.prefetched_docs_block = buildDocsPrefetchBlock(
            LLMServices::instance().searchIndex(), ctx_signals.retrieval_query,
            ctx_plan.docs_scope, ctx_plan.docs_top_k, 4096);
    }
    else if(provider_tools && intent == LLMIntentKind::Query)
    {
        const char* prefetch_env = std::getenv("NMSDK_LLM_QUERY_PREFETCH_DOCS");
        if(prefetch_env && prefetch_env[0] == '1' && LLMServices::instance().isInitialized())
        {
            ctx_input.prefetched_docs_block = buildDocsPrefetchBlock(
                LLMServices::instance().searchIndex(), planning_text, "docs", 3, 4096);
        }
    }

    if(ctx_plan.inject_link_patterns)
    {
        const ULinkPatternCatalog& link_cat = defaultLinkPatternCatalog();
        ctx_input.link_patterns_block =
            buildLinkPatternHintBlock(link_cat, ctx_signals.from_class, ctx_signals.to_class,
                                      ctx_plan.link_pattern_top_k);
        const ULLMConnectSemanticsCatalog& sem_cat = defaultConnectSemanticsCatalog();
        ctx_input.connect_semantics_block =
            buildConnectSemanticsHintBlock(sem_cat, ctx_signals.from_class, ctx_signals.to_class,
                                           ctx_plan.link_pattern_top_k);
    }

    if(isConnectGoalText(planning_text))
    {
        const std::string inspect = buildConnectInspectHintBlock();
        if(ctx_input.connect_semantics_block.empty())
            ctx_input.connect_semantics_block = inspect;
        else
            ctx_input.connect_semantics_block += "\n" + inspect;
    }

    ctx_input.allow_retriever_without_list_focus =
        ctx_plan.prefetch_snapshot && !req.gui.diagram_scope_long_name.empty();

    {
        nlohmann::json acquired_blocks = nlohmann::json::array();
        if(!ctx_input.link_patterns_block.empty())
            acquired_blocks.push_back("link_patterns");
        if(!ctx_input.connect_semantics_block.empty())
            acquired_blocks.push_back("connect_semantics");
        if(!ctx_input.prefetched_docs_block.empty())
            acquired_blocks.push_back("docs");
        if(ctx_input.allow_retriever_without_list_focus)
            acquired_blocks.push_back("retriever_diagram_scope");
        GetAuditLog().append("context_acquired",
                             {{"blocks", acquired_blocks},
                              {"from_class", ctx_signals.from_class},
                              {"to_class", ctx_signals.to_class}},
                             req.trace_id, req.session_id);
    }

    if(LLMServices::instance().isInitialized() && LLMServices::instance().projectContext())
    {
        ctx_input.long_term_memory_block = loadLongTermMemoryBlock(
            LLMServices::instance().projectContext()->paths().repository_root, session.user_id,
            req.gui.project_xml_path);
    }

    LLMCompletionOptions opts;
    if(LLMServices::instance().isInitialized())
    {
        opts.response_language = resolveResponseLanguage(
            LLMServices::instance().settings().runtime().preferred_response_language, "en");
    }
    else
        opts.response_language = "en";
    if(provider_tools)
        opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
    else
        opts.tools_for_api.clear();

    const bool thinking_enabled =
        m_provider.capabilities().supports_thinking
        && (!LLMServices::instance().isInitialized()
            || LLMServices::instance().settings().runtime().enable_ollama_thinking);
    if(thinking_enabled)
    {
        opts.think_mode = LLMThinkMode::On;
        if(opts.max_tokens < 8192)
            opts.max_tokens = 8192;
        if(const std::optional<std::string> model = modelOverrideForRoute(cortex_route))
            opts.model_override = *model;
    }

    // DD-THINK-003: never force tool_choice while thinking is on.
    if(provider_tools && !thinking_enabled
       && lifecycle_action != ConfigurationLifecycleAction::None
       && shouldForceLifecycleToolChoice(intent_result.confidence))
    {
        if(lifecycle_action == ConfigurationLifecycleAction::Load
           && wantsRecentConfiguration(req.user_text))
            opts.tool_choice = std::string("open_recent_configuration");
        else if(const std::optional<std::string> forced =
                    forcedToolForLifecycle(lifecycle_action, session.project_loaded))
            opts.tool_choice = *forced;
    }
    if(strict_plan_schema)
        opts.response_format = executionPlanOpenAiResponseFormat();
    const std::string user_lang = opts.response_language;
    ctx_input.response_language = opts.response_language;

    auto maybeExpandFromSearchTools = [&](const LLMToolCall& call, const ToolGatewayResult& tr) {
        if(call.name != "search_tools" || !tr.ok)
            return;
        expandToolFilterFromSearchResult(filter, tr.result, m_registry);
        ctx_input.tool_filter = filter;
        GetAuditLog().append("tool_filter_expanded",
                             {{"via", "search_tools"},
                              {"tools", tr.result.value("tools", nlohmann::json::array())}},
                             req.trace_id, req.session_id);
    };

    int tool_invocations = 0;
    const int max_tool_invocations = defaultPolicyLimits().max_tool_invocations_per_message;
    bool recovery_used = false;
    bool connect_recovery_used = false;
    bool calc_recovery_used = false;
    bool structure_recovery_used = false;
    bool watch_recovery_used = false;
    std::optional<std::pair<std::string, ToolGatewayResult>> last_graph_write;

    const bool is_cloud_profile = req.provider_profile.is_cloud;
    const int max_rounds = kMaxRounds;

    auto session_cancelled = [&]() {
        std::lock_guard<std::mutex> lock(m_cancel_mu);
        return m_cancelled || m_cancelled_sessions.find(req.session_id) != m_cancelled_sessions.end();
    };

    for(int round = 0; round < max_rounds && !session_cancelled(); ++round)
    {
        provider_messages = state.messages;

        LLMContextBudget budget;
        budget.compacted = context_compacted;
        prependEphemeralSystemMessages(provider_messages, ctx_input, &budget);
        if(round == 0 && !ctx_input.planning_text.empty()
           && isConnectGoalText(ctx_input.planning_text))
        {
            GetAuditLog().append("connect_semantics_context_injected",
                                 {{"goal_en_length", static_cast<int>(ctx_input.planning_text.size())}},
                                 req.trace_id, req.session_id);
        }
        GetAuditLog().append("context_budget",
                             {{"messages_chars", budget.messages_chars},
                              {"ephemeral_chars", budget.ephemeral_chars},
                              {"manifest_chars", budget.manifest_chars},
                              {"compacted", budget.compacted}},
                             req.trace_id, req.session_id);
        if(round == 0)
        {
            final.context_messages_chars = budget.messages_chars;
            final.context_ephemeral_chars = budget.ephemeral_chars;
            final.context_manifest_chars = budget.manifest_chars;
            final.context_compacted = budget.compacted;
        }

        if(round >= 1)
            opts.tool_choice.reset();
        if(is_cloud_profile)
        {
            ++state.cloud_provider_rounds;
            if(state.cloud_provider_rounds > defaultPolicyLimits().max_cloud_provider_rounds_per_session)
            {
                final.ok = false;
                final.error = "Cloud provider rate limit reached for this session (max "
                              + std::to_string(defaultPolicyLimits().max_cloud_provider_rounds_per_session)
                              + " rounds). Start a new session or use ollama-local.";
                setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return final;
            }
        }

        LLMCompletionResult completion;
        const bool use_stream =
            stream && stream->on_token && m_provider.capabilities().supports_streaming;
        if(use_stream)
        {
            if(stream->on_thinking_token)
                opts.on_thinking_chunk = [&](const std::string& token) {
                    if(!session_cancelled() && stream->on_thinking_token)
                        stream->on_thinking_token(token);
                };
            else
                opts.on_thinking_chunk = nullptr;
            m_provider.chatStream(
                provider_messages, opts,
                [&](const std::string& token) {
                    if(!session_cancelled() && stream->on_token)
                        stream->on_token(token);
                },
                [&](LLMCompletionResult r) { completion = std::move(r); });
        }
        else
        {
            opts.on_thinking_chunk = nullptr;
            completion = m_provider.chat(provider_messages, opts);
        }
        if(!completion.ok)
        {
            final.ok = false;
            final.error = completion.error_message;
            if(req.provider_profile.is_cloud)
            {
                final.error +=
                    " (tip: enable allow-cloud or switch to ollama-local / embedded-offline)";
            }
            else if(intent == LLMIntentKind::Mutate)
            {
                final.error +=
                    " (tip: use the component wizard or property editor for manual changes)";
            }
            else if(isOllamaProvider(req.provider_profile))
            {
                const std::string& err = completion.error_message;
                if(err.find("not found") != std::string::npos || err.find("HTTP 404") != std::string::npos)
                    final.error += formatOllamaModelMismatchHint(req.provider_profile);
                else
                    final.error += " (check: ollama serve, model pulled, Ollama 0.3+ for tools)";
            }
            assignTurnTerminal(final, TurnTerminal::ProviderError);
            setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            return final;
        }

        if(session_cancelled() && !completion.tool_calls.empty())
        {
            appendCancelledToolResults(m_store, req.session_id, completion.tool_calls);
            final.ok = false;
            final.text = "Turn cancelled.";
            assignTurnTerminal(final, TurnTerminal::Cancelled);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            m_store.persistToDisk(req.session_id);
            GetAuditLog().append("turn_cancelled_with_pending_tools",
                                 {{"tool_count", static_cast<int>(completion.tool_calls.size())}},
                                 req.trace_id, req.session_id);
            return final;
        }

        if(completion.tool_calls.empty() && intent == LLMIntentKind::Mutate && filter.include_write
           && provider_tools && state.workflow_phase != LLMWorkflowPhase::Understanding)
        {
            std::vector<LLMToolCall> embedded =
                tryExtractEmbeddedToolCalls(completion.text, m_registry);
            if(!embedded.empty())
            {
                completion.tool_calls = filterValidatedEmbeddedToolCalls(
                    std::move(embedded), m_registry, planning_text, req.trace_id, req.session_id);
            }
        }

        if(completion.tool_calls.empty())
        {
            if(last_graph_write)
            {
                final.ok = last_graph_write->second.ok;
                final.text = formatWriteToolUserMessage(last_graph_write->first,
                                                        last_graph_write->second);
                if(!final.ok && !last_graph_write->second.message.empty())
                    final.error = last_graph_write->second.message;
                assignTurnTerminal(final, TurnTerminal::Completed);
                setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return final;
            }

            if(shouldRequireActOrClarify(
                   provider_tools, true, planning_text, intent, lifecycle_action,
                   filter.include_write, static_cast<bool>(state.pending_tool_arguments),
                   state.workflow_phase == LLMWorkflowPhase::Understanding))
            {
                if(!recovery_used)
                {
                    recovery_used = true;
                    // TD-163: merge pack-scoped Act-or-Clarify recovery tools into allowlist.
                    // Empty matched_pack_ids → collectPackRecoveryTools returns tools from all packs.
                    nlohmann::json pack_recovery_json = nlohmann::json::array();
                    if(LLMServices::instance().isInitialized())
                    {
                        const std::vector<std::string> pack_recovery =
                            collectPackRecoveryTools(LLMServices::instance().packs(),
                                                     matched_pack_ids);
                        pack_recovery_json = pack_recovery;
                        if(!pack_recovery.empty())
                        {
                            mergePackToolNames(filter, pack_recovery);
                            opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
                            ctx_input.tool_filter = filter;
                        }
                    }
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    if(isChannelCalcGoalText(planning_text) || isChannelCalcGoalText(req.user_text))
                    {
                        recovery.content =
                            "Channel calculation goal: call start_channel_calculation "
                            "(or pause/reset/step_channel_calculation). Prefer channel_index=-1 "
                            "for all channels. Do not call ask_user unless arguments are missing. "
                            "If no tool can satisfy the request, reply exactly: NO_SUITABLE_TOOL.";
                    }
                    else if(isComponentStructureGoal(planning_text)
                            || isComponentStructureGoal(req.user_text))
                    {
                        recovery.content =
                            "Component structure goal: use describe_class / get_component_properties, "
                            "then set_property StructureBuildMode=2, NumSomaMembraneParts, "
                            "NumDendriteMembranePartsVec (space-separated), then calculate_component. "
                            "Do not add_component Dendrite classes. If no tool fits, reply exactly: "
                            "NO_SUITABLE_TOOL.";
                    }
                    else if(isWatchPlotGoal(planning_text) || isWatchPlotGoal(req.user_text))
                    {
                        recovery.content =
                            "Watch plot goal: call add_watch_series (surface=window) for each "
                            "signal. Nested roles (e.g. LT zone) use dotted long_name "
                            "Parent.Child with property on the child. Prefer create_watch_mdi only "
                            "when user asks for a separate Watches window. On ComponentNotFound "
                            "use find_component / get_net_snapshot — not search_project_docs. "
                            "Do not use open_component_gui_tab for plotting. If no tool fits, "
                            "reply exactly: NO_SUITABLE_TOOL.";
                    }
                    else
                    {
                        recovery.content =
                            "Actionable request detected. Call exactly one suitable tool, or "
                            "ask_user if arguments are missing. "
                            "If no tool can satisfy the request, reply exactly: NO_SUITABLE_TOOL. "
                            "Do not narrate topology or invent link results without tools.";
                    }
                    m_store.appendMessage(req.session_id, recovery);
                    GetAuditLog().append("act_or_clarify_recovery",
                                         {{"round", 1},
                                          {"intent", intent_name},
                                          {"matched_packs", matched_pack_ids},
                                          {"pack_recovery_tools", pack_recovery_json}},
                                         req.trace_id, req.session_id);
                    continue;
                }
                if((isChannelCalcGoalText(planning_text) || isChannelCalcGoalText(req.user_text))
                   && !calc_recovery_used)
                {
                    calc_recovery_used = true;
                    filter.include_write = true;
                    filter.allowed_tool_names = std::unordered_set<std::string>{
                        "start_channel_calculation",
                        "pause_channel_calculation",
                        "reset_channel_calculation",
                        "step_channel_calculation",
                        "list_channels",
                        "set_active_channel",
                        "ask_user",
                    };
                    opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
                    ctx_input.tool_filter = filter;
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    recovery.content =
                        "Calculation control only: call the matching *_channel_calculation tool "
                        "now (start/pause/reset/step). Do not invent status without a tool call.";
                    m_store.appendMessage(req.session_id, recovery);
                    GetAuditLog().append("channel_calc_recovery_round",
                                         {{"session_id", req.session_id}},
                                         req.trace_id, req.session_id);
                    continue;
                }
                if((isComponentStructureGoal(planning_text) || isComponentStructureGoal(req.user_text))
                   && !structure_recovery_used)
                {
                    structure_recovery_used = true;
                    filter.include_write = true;
                    filter.allowed_tool_names = std::unordered_set<std::string>{
                        "describe_class",
                        "get_component_properties",
                        "set_property",
                        "calculate_component",
                        "find_component",
                        "search_project_docs",
                        "ask_user",
                    };
                    opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
                    ctx_input.tool_filter = filter;
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    recovery.content =
                        "Structure recovery: set_property StructureBuildMode=2, "
                        "NumSomaMembraneParts=<N>, NumDendriteMembranePartsVec=\"L1 L2 …\", then "
                        "calculate_component. Do not add_component dendrites.";
                    m_store.appendMessage(req.session_id, recovery);
                    GetAuditLog().append("structure_recovery_round",
                                         {{"session_id", req.session_id}},
                                         req.trace_id, req.session_id);
                    continue;
                }
                if((isWatchPlotGoal(planning_text) || isWatchPlotGoal(req.user_text))
                   && !watch_recovery_used)
                {
                    watch_recovery_used = true;
                    filter.include_write = true;
                    filter.allowed_tool_names = std::unordered_set<std::string>{
                        "add_watch_series",
                        "list_watch_series",
                        "create_watch_mdi",
                        "list_watch_mdi",
                        "focus_watch_mdi",
                        "show_ui_panel",
                        "find_component",
                        "get_net_snapshot",
                        "get_component_ports",
                        "get_component_properties",
                        "ask_user",
                    };
                    opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
                    ctx_input.tool_filter = filter;
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    recovery.content =
                        "Watch recovery: call add_watch_series for each signal (surface=window). "
                        "Nested: long_name=Parent.Child, property_name on child. On miss use "
                        "find_component / get_net_snapshot (not docs). Do not open_component_gui_tab.";
                    m_store.appendMessage(req.session_id, recovery);
                    GetAuditLog().append("watch_plot_recovery_round",
                                         {{"session_id", req.session_id}},
                                         req.trace_id, req.session_id);
                    continue;
                }
                if(isConnectGoalText(planning_text) && !connect_recovery_used)
                {
                    connect_recovery_used = true;
                    // Ensure write tools are visible even if intent was misclassified earlier.
                    filter.include_write = true;
                    filter.allowed_tool_names = std::unordered_set<std::string>{
                        "connect_components",
                        "get_component_properties",
                        "get_component_ports",
                        "list_model_links",
                        "disconnect_components",
                        "ask_user",
                        "find_component",
                        "get_net_snapshot",
                        "search_project_docs",
                    };
                    opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
                    ctx_input.tool_filter = filter;
                    std::string hint =
                        "Connect/link goal: call list_model_links / get_component_ports to "
                        "inspect nested wiring, then connect_components. Do not add components. "
                        "Named components are subtree anchors.";
                    const ParsedConnectGoal parsed = parseConnectGoal(planning_text);
                    if(!parsed.explicit_links.empty())
                    {
                        const ConnectLinkSpec& link = parsed.explicit_links.front();
                        hint += " Suggested endpoints: from=" + link.from.token + " to="
                                + link.to.token + ".";
                    }
                    {
                        const DialogSlotMergeInput slot_in{
                            &state.messages,
                            state.pending_tool_arguments ? &*state.pending_tool_arguments : nullptr,
                            &req.gui, &m_registry};
                        const DialogSlotMergeResult slot = mergeDialogSlotsIntoToolArgs(slot_in);
                        nlohmann::json partial = slot.partial_arguments;
                        if(state.pending_tool_arguments
                           && state.pending_tool_arguments->partial_arguments.is_object())
                        {
                            for(auto it = state.pending_tool_arguments->partial_arguments.begin();
                                it != state.pending_tool_arguments->partial_arguments.end(); ++it)
                            {
                                if(!partial.contains(it.key()) || partial[it.key()].empty())
                                    partial[it.key()] = it.value();
                            }
                        }
                        if(partial.is_object() && !partial.empty())
                        {
                            hint += " Accumulated partial_arguments from dialog: "
                                    + partial.dump() + ".";
                        }
                    }
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    recovery.content = hint;
                    m_store.appendMessage(req.session_id, recovery);
                    GetAuditLog().append("connect_recovery_round",
                                         {{"session_id", req.session_id}},
                                         req.trace_id, req.session_id);
                    continue;
                }
                final.no_suitable_tool = true;
                final.text = formatUserMessage("error.no_suitable_tool", user_lang);
                assignTurnTerminal(final, TurnTerminal::Completed);
                GetAuditLog().append("act_or_clarify_exhausted",
                                     {{"intent", intent_name}},
                                     req.trace_id, req.session_id);
                setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return final;
            }

            LLMMessage assistant;
            assistant.role = LLMMessage::Role::Assistant;
            assistant.content = completion.text;
            if(!completion.thinking.empty())
                assistant.thinking = completion.thinking;
            m_store.appendMessage(req.session_id, assistant);
            final.text = completion.text;
            if(!completion.thinking.empty())
            {
                final.thinking = completion.thinking;
                // Soft safety cap only (~256KB); keep full thinking for AiChats / UI.
                constexpr std::size_t kMaxThinkingChars = 256u * 1024u;
                if(final.thinking.size() > kMaxThinkingChars)
                    final.thinking.resize(kMaxThinkingChars);
            }
            if(final.text.empty() && lifecycle_action != ConfigurationLifecycleAction::None)
            {
                if(const std::optional<ToolGatewayResult> open_tr = tryAutoOpenRecentAfterList(
                       req.user_text, app, m_store, req.session_id, m_gateway, m_registry, session,
                       req.trace_id, planning_text))
                {
                    final.ok = open_tr->ok;
                    final.text = formatLifecycleToolUserMessage("open_recent_configuration", *open_tr);
                    if(!open_tr->ok && !open_tr->message.empty())
                        final.error = open_tr->message;
                    if(open_tr->pending_confirmation)
                    {
                        final.pending_confirmation = true;
                        final.pending_confirmation_id = open_tr->confirmation_id;
                    }
                    setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                    setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }
                final.text =
                    "Could not complete the configuration command. Specify a folder or "
                    "project.ini path, or use File → Open Recent.";
            }
            if(intent == LLMIntentKind::Plan || task_path_decision.use_task_path)
            {
                if(auto plan = parseExecutionPlanFromAssistantText(completion.text))
                {
                    if(task_path_decision.use_task_path && plan->goal_en.empty())
                        plan->goal_en = planning_text;
                    if(task_path_decision.use_task_path && plan->confidence <= 0.f)
                        plan->confidence = 0.75f;

                    ULLMPolicyEngine policy;
                    const PolicyDecision plan_pol = policy.checkPlan(*plan, session, m_registry);
                    if(plan_pol.allowed)
                    {
                        const PlanConfirmDecision confirm_decision =
                            decidePlanConfirmation(*plan, session.autonomous_mode,
                                                   session.auto_apply_writes, false);
                        if(confirm_decision.needs_user_confirmation)
                        {
                            state.pending_plan = *plan;
                            final.pending_plan_execution = true;
                            final.pending_plan_id = plan->plan_id;
                            final.text =
                                formatExecutionPlanPreview(*plan)
                                + "\n\n[Plan ready — confirm execution in the assistant panel.]";
                            setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                            assignTurnTerminal(final, TurnTerminal::AwaitingConfirm);
                            m_store.persistToDisk(req.session_id);
                            return final;
                        }

                        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);
                        ULLMPlanExecutor executor(m_registry, m_gateway);
                        ULLMExecutionPlan run_plan = *plan;
                        PlanExecuteOptions plan_opts = planExecuteWithCheckpointOnFailure();
                        plan_opts.conversation_state = &state;
                        plan_opts.conversation_store = &m_store;
                        const PlanExecutionResult exec =
                            executor.execute(run_plan, session, req.trace_id, plan_opts);
                        final.ok = exec.ok;
                        final.text = exec.summary;
                        final.error = exec.ok ? "" : exec.summary;
                        if(!exec.ok && exec.paused_for_resume)
                        {
                            state.pending_plan = run_plan;
                            final.plan_paused = true;
                            final.can_resume_plan = true;
                            final.pending_plan_id = run_plan.plan_id;
                            setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                        }
                        else
                        {
                            setWorkflowPhase(state, exec.ok ? LLMWorkflowPhase::Completed
                                                            : LLMWorkflowPhase::Failed,
                                             req.trace_id);
                            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                        }
                        assignTurnTerminal(final, TurnTerminal::Completed);
                        m_store.persistToDisk(req.session_id);
                        return final;
                    }
                    final.text += "\n\n(Plan rejected by policy: " + plan_pol.deny_message + ")";
                }
            }
            assignTurnTerminal(final, TurnTerminal::Completed);
            setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            m_store.persistToDisk(req.session_id);
            return final;
        }

        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);
        GetAuditLog().append("tool_loop_entered",
                             {{"round", round},
                              {"tool_count", static_cast<int>(completion.tool_calls.size())}},
                             req.trace_id, req.session_id);

        LLMMessage assistant_tools;
        assistant_tools.role = LLMMessage::Role::Assistant;
        assistant_tools.content = completion.text;
        if(!completion.thinking.empty())
            assistant_tools.thinking = completion.thinking;
        assistant_tools.assistant_tool_calls = completion.tool_calls;
        m_store.appendMessage(req.session_id, assistant_tools);
        if(!completion.thinking.empty())
        {
            final.thinking = completion.thinking;
            constexpr std::size_t kMaxThinkingChars = 256u * 1024u;
            if(final.thinking.size() > kMaxThinkingChars)
                final.thinking.resize(kMaxThinkingChars);
        }

        const IntentAmbiguityDecision ambiguity =
            evaluateIntentAmbiguity(state, intent_result, completion.tool_calls, m_registry);
        if(ambiguity.block_writes)
        {
            GetAuditLog().append("intent_ambiguity_blocked",
                                 {{"reason_code", ambiguity.reason_code}}, req.trace_id,
                                 req.session_id);
            GetAuditLog().append("false_execution_prevented",
                                 {{"reason_code", ambiguity.reason_code}}, req.trace_id,
                                 req.session_id);
            LLMFinalResponse blocked;
            blocked.ok = true;
            blocked.needs_argument_clarification = true;
            blocked.text = ambiguity.user_message;
            LLMMessage assistant_msg;
            assistant_msg.role = LLMMessage::Role::Assistant;
            assistant_msg.content = blocked.text;
            m_store.appendMessage(req.session_id, assistant_msg);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            m_store.persistToDisk(req.session_id);
            return blocked;
        }

        auto invokeOne = [&](const LLMToolCall& call) -> std::pair<LLMToolCall, ToolGatewayResult> {
            if(tool_invocations >= max_tool_invocations)
            {
                ToolGatewayResult limited;
                limited.ok = false;
                limited.error_code = "TOOL_LIMIT";
                limited.message = "Maximum tool invocations per message reached";
                return {call, limited};
            }
            ++tool_invocations;
            if(session.autonomous_mode != LLMAutonomousMode::Off)
            {
                const int max_auto_steps = LLMServices::instance().isInitialized()
                                               ? LLMServices::instance()
                                                     .settings()
                                                     .runtime()
                                                     .max_autonomous_steps
                                               : defaultPolicyLimits().max_autonomous_steps_per_message;
                const AutonomousStepDecision auto_decision = ULLMAutonomousPolicy::checkStep(
                    call.name, session.autonomous_mode, session.autonomous_steps_taken,
                    max_auto_steps);
                if(!auto_decision.allowed)
                {
                    ToolGatewayResult denied;
                    denied.ok = false;
                    denied.error_code = auto_decision.deny_code;
                    denied.message = auto_decision.deny_message;
                    GetAuditLog().append("autonomous_step_denied",
                                         {{"tool_name", call.name},
                                          {"code", auto_decision.deny_code}},
                                         req.trace_id, req.session_id);
                    return {call, denied};
                }
            }
            const LLMToolDefinition* call_def = m_registry.find(call.name);
            if(call_def && call_def->kind == LLMToolKind::Write
               && state.intent_contract_kind != LLMIntentKind::Mutate)
            {
                ToolGatewayResult denied;
                denied.ok = false;
                denied.error_code = "INTENT_CONTRACT_MISMATCH";
                denied.message =
                    "Write tool call blocked by intent contract (current intent is not mutate)";
                GetAuditLog().append("intent_contract_denied",
                                     {{"tool_name", call.name}, {"kind", intent_name}}, req.trace_id,
                                     req.session_id);
                return {call, denied};
            }
            if(call.name == "propose_plan")
            {
                ToolGatewayResult tr;
                if(const std::optional<ULLMExecutionPlan> plan =
                       executionPlanFromProposePlanArguments(call.arguments))
                {
                    ULLMPolicyEngine policy;
                    const PolicyDecision plan_pol = policy.checkPlan(*plan, session, m_registry);
                    if(plan_pol.allowed)
                    {
                        state.pending_plan = *plan;
                        tr.ok = true;
                        tr.result = {{"plan_id", plan->plan_id}, {"step_count", plan->steps.size()}};
                        tr.message = formatExecutionPlanPreview(*plan)
                                       + "\n\n[Plan ready — confirm execution in the assistant panel.]";
                    }
                    else
                    {
                        tr.ok = false;
                        tr.error_code = "PLAN_POLICY_DENIED";
                        tr.message = "Plan rejected by policy: " + plan_pol.deny_message;
                    }
                }
                else
                {
                    tr.ok = false;
                    tr.error_code = "INVALID_PLAN";
                    tr.message = "propose_plan requires non-empty goal and steps with tool_name.";
                }
                return {call, tr};
            }
            if(call.name == "ask_user")
            {
                PendingUserQuestion pq;
                pq.question_id = call.id.empty() ? req.trace_id : call.id;
                pq.prompt = call.arguments.value("question", std::string());
                pq.choices = call.arguments.value("choices", nlohmann::json::array());
                pq.allow_free_text = call.arguments.value("allow_free_text", true);
                const std::string prompt = pq.prompt;
                const std::string question_id = pq.question_id;
                state.pending_user_question = std::move(pq);
                setWorkflowPhase(state, LLMWorkflowPhase::AwaitingUserInput, req.trace_id);
                ToolGatewayResult tr;
                tr.ok = true;
                tr.message = prompt;
                GetAuditLog().append("ask_user_issued", {{"question_id", question_id}}, req.trace_id,
                                     req.session_id);
                return {call, tr};
            }
            ToolInvokeRequest invoke;
            invoke.trace_id = req.trace_id;
            invoke.tool_name = call.name;
            invoke.arguments = call.arguments;
            invoke.idempotency_key =
                makeIdempotencyKey(req.session_id, req.trace_id, call.name, call.arguments, call.id);
            const ConfigurationLifecycleAction call_action = lifecycleActionFromToolName(call.name);
            if(call_action != ConfigurationLifecycleAction::None)
            {
                if(const LLMToolDefinition* def = m_registry.find(call.name))
                {
                    const nlohmann::json props =
                        def->input_schema.value("properties", nlohmann::json::object());
                    if(props.is_object() && !invoke.arguments.empty())
                    {
                        nlohmann::json filtered = nlohmann::json::object();
                        for(auto it = invoke.arguments.begin(); it != invoke.arguments.end(); ++it)
                        {
                            if(props.contains(it.key()))
                                filtered[it.key()] = it.value();
                        }
                        invoke.arguments = std::move(filtered);
                    }
                }
                PendingToolArguments bootstrap;
                bootstrap.tool_name = call.name;
                bootstrap.action = call_action;
                bootstrap.partial_arguments = invoke.arguments;
                invoke.arguments = mergeArgumentsFromUserText(bootstrap, req.user_text, app);
            }
            invoke.session = session;
            invoke.user_text_hint = entity_user_text_hint;
            if(call_def && call_def->kind == LLMToolKind::Write)
            {
                WriteToolExecutionRequest wreq;
                wreq.session_id = req.session_id;
                wreq.trace_id = req.trace_id;
                wreq.tool_name = call.name;
                wreq.arguments = invoke.arguments;
                wreq.session = session;
                wreq.gui = guiSnapshotForWrite(state, req.gui);
                wreq.user_lang = user_lang;
                wreq.user_text_hint = entity_user_text_hint;
                wreq.idempotency_action_id = call.id;
                const WriteToolExecutionResult wres =
                    executeWriteWithPreviewAndVerify(*this, state, wreq);
                ToolGatewayResult tr = wres.gateway;
                if(tr.ok && call.name == "set_active_channel"
                   && call.arguments.contains("channel_index"))
                    session.active_channel_index = call.arguments["channel_index"].get<int>();
                if(tr.ok && !tr.pending_confirmation
                   && session.autonomous_mode != LLMAutonomousMode::Off
                   && ULLMAutonomousPolicy::isAutonomousWriteTool(call.name))
                    ++session.autonomous_steps_taken;
                return {call, tr};
            }
            ToolGatewayResult tr = m_gateway.invoke(invoke);
            if(tr.ok && call.name == "set_active_channel" && call.arguments.contains("channel_index"))
                session.active_channel_index = call.arguments["channel_index"].get<int>();
            if(tr.ok && !tr.pending_confirmation && session.autonomous_mode != LLMAutonomousMode::Off
               && ULLMAutonomousPolicy::isAutonomousWriteTool(call.name))
                ++session.autonomous_steps_taken;
            return {call, tr};
        };

        const bool all_read = std::all_of(
            completion.tool_calls.begin(), completion.tool_calls.end(),
            [&](const LLMToolCall& call) {
                if(call.name == "ask_user" || call.name == "propose_plan"
                   || call.name == "spawn_explore_subagent")
                    return false;
                const LLMToolDefinition* def = m_registry.find(call.name);
                return def && def->kind == LLMToolKind::Read;
            });

        if(m_system_log_reader)
            m_system_log_reader->mark();

        if(all_read && completion.tool_calls.size() > 1)
        {
            std::vector<std::future<std::pair<LLMToolCall, ToolGatewayResult>>> futures;
            futures.reserve(completion.tool_calls.size());
            for(const LLMToolCall& call : completion.tool_calls)
                futures.push_back(std::async(std::launch::async, invokeOne, call));
            // Invariant (TD-163): append tool results in original tool_calls order.
            // futures[i] was pushed for tool_calls[i]; sequential fut.get() preserves that index order.
            for(auto& fut : futures)
            {
                auto [call, tr] = fut.get();
                nlohmann::json disambiguation;
                if(extractToolDisambiguationPayload(tr, disambiguation))
                {
                    if(disambiguation.value("kind", "") == "class")
                        return routeClarificationOrDisambiguation(
                            state, req.trace_id, call, PendingDisambiguationKind::Class,
                            "class_name", disambiguation);
                    return routeClarificationOrDisambiguation(
                        state, req.trace_id, call, PendingDisambiguationKind::Component,
                        disambiguation.value("field", "long_name"), disambiguation);
                }
                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call.id;
                tool_msg.tool_name = call.name;
                tool_msg.content =
                    buildToolMessageContent(tr, m_system_log_reader.get(), session.active_channel_index);
                m_store.appendMessage(req.session_id, tool_msg);
                capturePendingOpenRecentAfterList(m_store, req.session_id, call.name, tr);
                maybeExpandFromSearchTools(call, tr);
                if(call.name == "ask_user" && state.pending_user_question)
                {
                    final.ok = true;
                    final.awaiting_user_input = true;
                    final.pending_question_id = state.pending_user_question->question_id;
                    final.user_choice_options = state.pending_user_question->choices;
                    final.text = state.pending_user_question->prompt;
                    assignTurnTerminal(final, TurnTerminal::AwaitingUser);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }
                if(call.name == "propose_plan" && tr.ok && state.pending_plan)
                {
                    final.ok = true;
                    final.pending_plan_execution = true;
                    final.pending_plan_id = state.pending_plan->plan_id;
                    final.text = tr.message;
                    setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                    assignTurnTerminal(final, TurnTerminal::AwaitingConfirm);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }
            }
        }
        else
        {
            std::optional<std::pair<std::string, ToolGatewayResult>> lifecycle_write_done;
            for(const LLMToolCall& call : completion.tool_calls)
            {
                const auto [call_copy, tr] = invokeOne(call);
                nlohmann::json disambiguation_early;
                if(extractToolDisambiguationPayload(tr, disambiguation_early)
                   && disambiguation_early.value("kind", "") == "class")
                    return routeClarificationOrDisambiguation(
                        state, req.trace_id, call_copy, PendingDisambiguationKind::Class,
                        "class_name", disambiguation_early);
                if(extractToolDisambiguationPayload(tr, disambiguation_early)
                   && disambiguation_early.value("kind", "") == "component")
                    return routeClarificationOrDisambiguation(
                        state, req.trace_id, call_copy, PendingDisambiguationKind::Component,
                        disambiguation_early.value("field", "long_name"), disambiguation_early);
                const std::vector<ToolArgumentFieldSpec> invoke_missing =
                    findMissingArgumentsForTool(call_copy.name, call_copy.arguments, app,
                                                m_registry);
                if(!invoke_missing.empty()
                   && lifecycleActionFromToolName(call_copy.name)
                          != ConfigurationLifecycleAction::None)
                {
                    PendingToolArguments pending;
                    pending.tool_name = call_copy.name;
                    pending.action = lifecycleActionFromToolName(call_copy.name);
                    pending.partial_arguments = call_copy.arguments;
                    pending.missing_fields = invoke_missing;
                    GetAuditLog().append("lifecycle_args_requested",
                                         {{"tool_name", call_copy.name},
                                          {"reason", "missing_fields"}},
                                         req.trace_id, req.session_id);
                    return returnArgumentRequest(state, req.trace_id, pending, app);
                }
                if(shouldPromptForMissingToolArguments(call_copy.name, tr, invoke_missing))
                {
                    PendingToolArguments pending;
                    pending.tool_name = call_copy.name;
                    pending.action = lifecycleActionFromToolName(call_copy.name);
                    pending.partial_arguments = call_copy.arguments;

                    if(pending.action != ConfigurationLifecycleAction::None)
                    {
                        nlohmann::json merged =
                            mergeArgumentsFromUserText(pending, req.user_text, app);
                        std::vector<ToolArgumentFieldSpec> merged_missing =
                            findMissingArgumentsForTool(call_copy.name, merged, app, m_registry);
                        if(merged_missing.empty())
                        {
                            ToolInvokeRequest retry_req;
                            retry_req.trace_id = req.trace_id;
                            retry_req.tool_name = call_copy.name;
                            retry_req.arguments = merged;
                            retry_req.idempotency_key =
                                makeIdempotencyKey(req.session_id, req.trace_id, call_copy.name, merged,
                                                   call_copy.id.empty() ? "retry" : call_copy.id + ":retry");
                            retry_req.session = session;
                            retry_req.user_text_hint = entity_user_text_hint;
                            const LLMToolDefinition* retry_def = m_registry.find(call_copy.name);
                            ToolGatewayResult retry_tr;
                            if(retry_def && retry_def->kind == LLMToolKind::Write)
                            {
                                WriteToolExecutionRequest wreq;
                                wreq.session_id = req.session_id;
                                wreq.trace_id = req.trace_id;
                                wreq.tool_name = call_copy.name;
                                wreq.arguments = retry_req.arguments;
                                wreq.session = session;
                                wreq.gui = guiSnapshotForWrite(state, req.gui);
                                wreq.user_lang = user_lang;
                                wreq.user_text_hint = entity_user_text_hint;
                                wreq.idempotency_action_id =
                                    call_copy.id.empty() ? "retry" : call_copy.id + ":retry";
                                const WriteToolExecutionResult wres =
                                    executeWriteWithPreviewAndVerify(*this, state, wreq);
                                retry_tr = wres.gateway;
                            }
                            else
                                retry_tr = m_gateway.invoke(retry_req);
                            if(retry_tr.ok)
                            {
                                if(retry_tr.pending_confirmation)
                                {
                                    setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                                    PendingConfirmation pending_confirm;
                                    pending_confirm.confirmation_id = retry_tr.confirmation_id;
                                    pending_confirm.created_at_unix_sec = confirmationNowUnixSec();
                                    pending_confirm.request = ToolInvokeRequest{};
                                    pending_confirm.request.trace_id = req.trace_id;
                                    pending_confirm.request.tool_name = call_copy.name;
                                    pending_confirm.request.arguments = retry_req.arguments;
                                    pending_confirm.request.session = session;
                                    pending_confirm.request.confirmed = true;
                                    m_store.setPending(req.session_id, pending_confirm);
                                    final.pending_confirmation = true;
                                    final.pending_confirmation_id = retry_tr.confirmation_id;
                                    final.action_preview_text = formatActionIntentPreview(
                                        call_copy.name, retry_req.arguments, req.gui, session, user_lang);
                                    final.text = formatHitlConfirmationText(final.action_preview_text,
                                                                            user_lang);
                                    m_store.persistToDisk(req.session_id);
                                    return final;
                                }

                                LLMMessage tool_msg;
                                tool_msg.role = LLMMessage::Role::Tool;
                                tool_msg.tool_call_id = call_copy.id;
                                tool_msg.tool_name = call_copy.name;
                                tool_msg.content = buildToolMessageContent(
                                    retry_tr, m_system_log_reader.get(), session.active_channel_index);
                                m_store.appendMessage(req.session_id, tool_msg);

                                if(isLifecycleWriteToolName(call_copy.name))
                                    lifecycle_write_done = std::make_pair(call_copy.name, retry_tr);
                                continue;
                            }
                        }
                    }

                    pending.missing_fields = invoke_missing;
                    if(pending.missing_fields.empty()
                       && pending.action != ConfigurationLifecycleAction::None)
                        pending.missing_fields = argumentFieldsForLifecycle(pending.action);
                    GetAuditLog().append("lifecycle_args_requested",
                                         {{"tool_name", call_copy.name},
                                          {"error_code", tr.error_code}},
                                         req.trace_id, req.session_id);
                    return returnArgumentRequest(state, req.trace_id, pending, app);
                }
                if(toolInvokeNeedsArgumentClarification(call_copy.name, tr))
                {
                    LLMMessage tool_msg;
                    tool_msg.role = LLMMessage::Role::Tool;
                    tool_msg.tool_call_id = call_copy.id;
                    tool_msg.tool_name = call_copy.name;
                    tool_msg.content = buildToolMessageContent(
                        tr, m_system_log_reader.get(), session.active_channel_index);
                    m_store.appendMessage(req.session_id, tool_msg);
                    continue;
                }
                if(tr.pending_confirmation)
                {
                    setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                    PendingConfirmation pending;
                    pending.confirmation_id = tr.confirmation_id;
                    pending.created_at_unix_sec = confirmationNowUnixSec();
                    pending.request = ToolInvokeRequest{};
                    pending.request.trace_id = req.trace_id;
                    pending.request.tool_name = call_copy.name;
                    pending.request.arguments = call_copy.arguments;
                    pending.request.session = session;
                    pending.request.confirmed = true;
                    m_store.setPending(req.session_id, pending);
                    final.pending_confirmation = true;
                    final.pending_confirmation_id = tr.confirmation_id;
                    final.action_preview_text = formatActionIntentPreview(
                        call_copy.name, call_copy.arguments, req.gui, session, user_lang);
                    final.text =
                        formatHitlConfirmationText(final.action_preview_text, user_lang);
                    GetAuditLog().append("escalation_to_hitl",
                                         {{"tool_name", call_copy.name},
                                          {"confirmation_id", tr.confirmation_id}},
                                         req.trace_id, req.session_id);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }

                nlohmann::json disambiguation;
                if(extractToolDisambiguationPayload(tr, disambiguation))
                {
                    if(disambiguation.value("kind", "") == "class")
                        return routeClarificationOrDisambiguation(
                            state, req.trace_id, call_copy, PendingDisambiguationKind::Class,
                            "class_name", disambiguation);
                    return routeClarificationOrDisambiguation(
                        state, req.trace_id, call_copy, PendingDisambiguationKind::Component,
                        disambiguation.value("field", "long_name"), disambiguation);
                }

                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call_copy.id;
                tool_msg.tool_name = call_copy.name;
                tool_msg.content =
                    buildToolMessageContent(tr, m_system_log_reader.get(), session.active_channel_index);
                m_store.appendMessage(req.session_id, tool_msg);
                maybeExpandFromSearchTools(call_copy, tr);
                if(call_copy.name == "ask_user" && state.pending_user_question)
                {
                    final.ok = true;
                    final.awaiting_user_input = true;
                    final.pending_question_id = state.pending_user_question->question_id;
                    final.user_choice_options = state.pending_user_question->choices;
                    final.text = state.pending_user_question->prompt;
                    m_store.persistToDisk(req.session_id);
                    return final;
                }
                capturePendingOpenRecentAfterList(m_store, req.session_id, call_copy.name, tr);
                if(call_copy.name == "list_recent_configurations" && tr.ok)
                {
                    if(const std::optional<ToolGatewayResult> open_tr = tryAutoOpenRecentAfterList(
                           req.user_text, app, m_store, req.session_id, m_gateway, m_registry,
                           session, req.trace_id, planning_text))
                    {
                        LLMMessage open_msg;
                        open_msg.role = LLMMessage::Role::Tool;
                        open_msg.tool_call_id = call_copy.id + ":open_recent";
                        open_msg.tool_name = "open_recent_configuration";
                        open_msg.content = buildToolMessageContent(
                            *open_tr, m_system_log_reader.get(), session.active_channel_index);
                        m_store.appendMessage(req.session_id, open_msg);
                        if(open_tr->pending_confirmation)
                        {
                            setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation,
                                             req.trace_id);
                            PendingConfirmation pending;
                            pending.confirmation_id = open_tr->confirmation_id;
                            pending.created_at_unix_sec = confirmationNowUnixSec();
                            pending.request = ToolInvokeRequest{};
                            pending.request.trace_id = req.trace_id;
                            pending.request.tool_name = "open_recent_configuration";
                            pending.request.arguments = nlohmann::json::object();
                            pending.request.session = session;
                            pending.request.confirmed = true;
                            m_store.setPending(req.session_id, pending);
                            final.pending_confirmation = true;
                            final.pending_confirmation_id = open_tr->confirmation_id;
                            final.action_preview_text = formatActionIntentPreview(
                                "open_recent_configuration", nlohmann::json::object(), req.gui, session,
                                user_lang);
                            final.text =
                                formatHitlConfirmationText(final.action_preview_text, user_lang);
                            m_store.persistToDisk(req.session_id);
                            return final;
                        }
                        if(isLifecycleWriteToolName("open_recent_configuration"))
                            lifecycle_write_done = std::make_pair("open_recent_configuration", *open_tr);
                    }
                }

                const LLMToolDefinition* write_def = m_registry.find(call_copy.name);
                if(write_def && write_def->kind == LLMToolKind::Write && !tr.pending_confirmation)
                    last_graph_write = {call_copy.name, tr};

                const bool block_early_exit = task_path_decision.use_task_path;

                if(!block_early_exit && completion.tool_calls.size() == 1
                   && call_copy.name == "add_component" && !tr.pending_confirmation)
                {
                    final.ok = tr.ok;
                    final.action_preview_text = formatActionIntentPreview(
                        call_copy.name, call_copy.arguments, req.gui, session, user_lang);
                    final.text = combinePreviewAndOutcome(
                        final.action_preview_text, formatWriteToolUserMessage(call_copy.name, tr));
                    if(!final.ok && !tr.message.empty())
                        final.error = tr.message;
                    assignTurnTerminal(final, TurnTerminal::Completed);
                    setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                    setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }

                if(lifecycle_action != ConfigurationLifecycleAction::None
                   && isLifecycleWriteToolName(call_copy.name) && tr.ok && !tr.pending_confirmation
                   && invoke_missing.empty())
                    lifecycle_write_done = std::make_pair(call_copy.name, tr);
            }

            if(lifecycle_write_done)
            {
                final.ok = lifecycle_write_done->second.ok;
                final.text = formatLifecycleToolUserMessage(lifecycle_write_done->first,
                                                            lifecycle_write_done->second);
                if(!final.ok && !lifecycle_write_done->second.message.empty())
                    final.error = lifecycle_write_done->second.message;
                setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return final;
            }
        }

        if(provider_tools)
        {
            ctx_input.tool_filter = filter;
            opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
        }
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
    if(lifecycle_action == ConfigurationLifecycleAction::Create)
    {
        final.text =
            "Could not create a configuration automatically. Specify a folder, e.g. "
            "\"create config in /tmp/MyProject\", or use File → New in the menu.";
    }
    else if(lifecycle_action != ConfigurationLifecycleAction::None)
    {
        final.text =
            "The request needed too many tool steps. Try a shorter command with an explicit path, "
            "or use the File menu.";
    }
    else
        final.text = formatUserMessage("error.max_rounds", user_lang);
    assignTurnTerminal(final, TurnTerminal::MaxRounds);
    m_store.persistToDisk(req.session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::confirmPending(const std::string& session_id,
                                                      const std::string& confirmation_id)
{
    LLMFinalResponse final;
    SessionBusyScope busy(*this, session_id, final);
    if(!busy)
        return final;

    ConversationState& state = m_store.getOrCreate(session_id);
    const int confirmation_ttl = defaultPolicyLimits().confirmation_ttl_seconds;
    if(m_store.expirePendingIfStale(session_id, confirmation_ttl))
    {
        GetAuditLog().append("confirmation_expired",
                             {{"reason", "ttl_on_confirm"},
                              {"ttl_seconds", confirmation_ttl}},
                             "", session_id);
        final.ok = false;
        final.error = "Confirmation expired (limit " + std::to_string(confirmation_ttl / 60)
                      + " min). Please ask again.";
        return final;
    }
    if(!state.pending || state.pending->confirmation_id != confirmation_id)
    {
        final.ok = false;
        final.error = "No matching pending confirmation";
        return final;
    }
    snapshotLastSessionContext(state, state.pending->request.session);
    ToolInvokeRequest pending_req = state.pending->request;
    pending_req.confirmed = true;
    pending_req.idempotency_key =
        makeIdempotencyKey(session_id, confirmation_id, pending_req.tool_name, pending_req.arguments,
                           confirmation_id);

    std::string user_lang = "en";
    if(LLMServices::instance().isInitialized())
        user_lang = resolveResponseLanguage(
            LLMServices::instance().settings().runtime().preferred_response_language, "en");

    LLMGuiContextSnapshot gui;
    if(const LLMGuiContextSnapshot* pin_gui = guiContextForWrite(state))
        gui = *pin_gui;
    else if(state.last_gui_context)
        gui = *state.last_gui_context;

    WriteToolExecutionRequest wreq;
    wreq.session_id = session_id;
    wreq.trace_id = pending_req.trace_id;
    wreq.tool_name = pending_req.tool_name;
    wreq.arguments = pending_req.arguments;
    wreq.session = pending_req.session;
    wreq.gui = gui;
    wreq.user_lang = user_lang;
    wreq.user_text_hint = pending_req.user_text_hint;
    wreq.idempotency_action_id = confirmation_id;
    wreq.skip_preview = true;
    wreq.confirmed = true;
    const WriteToolExecutionResult wres =
        executeWriteWithPreviewAndVerify(*this, state, wreq);
    const ToolGatewayResult& tr = wres.gateway;
    const std::string pending_tool_call_id = state.pending->tool_call_id;

    m_store.clearPending(session_id);
    final.ok = tr.ok;
    final.text = combinePreviewAndOutcome(wres.preview_text, wres.outcome_text);
    if(!tr.ok)
        final.error = tr.message;
    if(tr.ok)
    {
        std::string tool_call_id = pending_tool_call_id;
        if(tool_call_id.empty())
        {
            tool_call_id = confirmation_id;
            for(auto it = state.messages.rbegin(); it != state.messages.rend(); ++it)
            {
                if(it->role != LLMMessage::Role::Assistant || !it->assistant_tool_calls)
                    continue;
                for(const LLMToolCall& tc : *it->assistant_tool_calls)
                {
                    if(tc.name == pending_req.tool_name)
                    {
                        tool_call_id = tc.id;
                        break;
                    }
                }
                break;
            }
        }
        appendToolResultOnly(
            m_store, session_id, tool_call_id, pending_req.tool_name,
            buildToolMessageContent(tr, m_system_log_reader.get(),
                                    pending_req.session.active_channel_index));
        if(!final.text.empty())
        {
            LLMMessage outcome;
            outcome.role = LLMMessage::Role::Assistant;
            outcome.content = final.text;
            m_store.appendMessage(session_id, outcome);
        }
    }
    m_store.persistToDisk(session_id);
    final.tool_trace = state.current_turn_tool_trace;
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::confirmPlanExecution(const std::string& session_id,
                                                             const std::string& trace_id,
                                                             const LLMSessionContext& session)
{
    LLMFinalResponse final;
    SessionBusyScope busy(*this, session_id, final);
    if(!busy)
        return final;

    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan)
    {
        final.ok = false;
        final.error = "No pending execution plan";
        return final;
    }

    snapshotLastSessionContext(state, session);

    ULLMPolicyEngine policy;
    const PolicyDecision plan_pol = policy.checkPlan(*state.pending_plan, session, m_registry);

    if(!plan_pol.allowed)
    {
        final.ok = false;
        final.error = plan_pol.deny_message;
        return final;
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Executing, trace_id);
    ULLMPlanExecutor executor(m_registry, m_gateway);
    ULLMExecutionPlan plan = *state.pending_plan;
    const std::optional<int> session_qty =
        state.last_quantity.valid ? std::optional<int>(state.last_quantity.primary) : std::nullopt;
    applyGoalQuantityToExecutionPlan(plan, session_qty);
    PlanExecuteOptions plan_opts = planExecuteWithCheckpointOnFailure();
    plan_opts.conversation_state = &state;
    plan_opts.conversation_store = &m_store;
    const PlanExecutionResult exec = executor.execute(plan, session, trace_id, plan_opts);

    if(exec.ok)
    {
        state.pending_plan.reset();
        final.ok = true;
        final.text = exec.summary;
        appendAgentNote(state, "Plan executed: " + plan.plan_id);
        setWorkflowPhase(state, LLMWorkflowPhase::Completed, trace_id);
    }
    else if(exec.paused_for_resume)
    {
        plan.paused = true;
        state.pending_plan = plan;
        final.ok = false;
        final.text = exec.summary;
        final.error = exec.summary;
        final.plan_paused = true;
        final.can_resume_plan = true;
        final.pending_plan_id = plan.plan_id;
        setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, trace_id);
    }
    else
    {
        state.pending_plan.reset();
        final.ok = false;
        final.text = exec.summary;
        final.error = exec.compensation_note.empty() ? exec.summary : exec.compensation_note;
        setWorkflowPhase(state, LLMWorkflowPhase::Failed, trace_id);
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    m_store.persistToDisk(session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::resumePlanExecution(const std::string& session_id,
                                                              const std::string& trace_id,
                                                              const LLMSessionContext& session)
{
    LLMFinalResponse final;
    SessionBusyScope busy(*this, session_id, final);
    if(!busy)
        return final;

    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan || !state.pending_plan->paused)
    {
        final.ok = false;
        final.error = "No paused plan to resume";
        return final;
    }

    snapshotLastSessionContext(state, session);

    ULLMPolicyEngine policy;
    const PolicyDecision plan_pol = policy.checkPlan(*state.pending_plan, session, m_registry);
    if(!plan_pol.allowed)
    {
        final.ok = false;
        final.error = plan_pol.deny_message;
        return final;
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Executing, trace_id);
    ULLMPlanExecutor executor(m_registry, m_gateway);
    ULLMExecutionPlan plan = *state.pending_plan;
    PlanExecuteOptions plan_opts = planExecuteResume();
    plan_opts.conversation_state = &state;
    plan_opts.conversation_store = &m_store;
    const PlanExecutionResult exec = executor.execute(plan, session, trace_id, plan_opts);

    if(exec.ok)
    {
        state.pending_plan.reset();
        final.ok = true;
        final.text = exec.summary;
    }
    else if(exec.paused_for_resume)
    {
        plan.paused = true;
        state.pending_plan = plan;
        final.ok = false;
        final.text = exec.summary;
        final.plan_paused = true;
        final.can_resume_plan = true;
        final.pending_plan_id = plan.plan_id;
    }
    else
    {
        state.pending_plan = plan;
        final.ok = false;
        final.text = exec.summary;
        final.error = exec.summary;
        final.plan_paused = true;
        final.can_resume_plan = true;
        final.pending_plan_id = plan.plan_id;
    }

    m_store.persistToDisk(session_id);
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::rollbackPlanExecution(const std::string& session_id,
                                                                const std::string& trace_id,
                                                                const LLMSessionContext& session)
{
    LLMFinalResponse final;
    SessionBusyScope busy(*this, session_id, final);
    if(!busy)
        return final;

    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan)
    {
        final.ok = false;
        final.error = "No plan to rollback";
        return final;
    }

    snapshotLastSessionContext(state, session);

    ULLMPlanExecutor executor(m_registry, m_gateway);
    std::string note;
    const int expected = rollbackEligibleWriteCount(*state.pending_plan);
    const int applied =
        executor.compensateCompletedWrites(*state.pending_plan, session, trace_id, note, &state,
                                           &m_store);
    std::string rollback_status = "rollback_failed";
    if(expected == 0)
        rollback_status = "rolled_back_nothing_to_compensate";
    else if(applied >= expected)
        rollback_status = "rolled_back";
    else if(applied > 0)
        rollback_status = "partial_rollback";
    GetAuditLog().append("plan_rollback",
                         {{"plan_id", state.pending_plan->plan_id},
                          {"applied", applied},
                          {"expected", expected},
                          {"status", rollback_status},
                          {"note", note}},
                         trace_id, session_id);

    state.pending_plan.reset();
    final.rollback_status = rollback_status;
    final.ok = rollback_status == "rolled_back" || rollback_status == "rolled_back_nothing_to_compensate";
    if(rollback_status == "rolled_back")
        final.text = note.empty() ? "Plan rolled back." : "Plan rolled back. " + note;
    else if(rollback_status == "rolled_back_nothing_to_compensate")
        final.text = "Plan discarded (no completed write steps to rollback).";
    else if(rollback_status == "partial_rollback")
    {
        final.text = "Plan rollback partially applied.";
        final.error = "Only " + std::to_string(applied) + " of " + std::to_string(expected)
                      + " rollback action(s) succeeded.";
    }
    else
    {
        final.text = "Plan rollback failed.";
        final.error = "No rollback actions were applied.";
    }
    m_store.persistToDisk(session_id);
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::returnDisambiguationRequest(
    ConversationState& state, const std::string& trace_id, const LLMToolCall& call,
    PendingDisambiguationKind kind, const std::string& field_name,
    const nlohmann::json& disambiguation, bool include_candidate_list)
{
    LLMFinalResponse final;
    PendingToolArguments pending;
    pending.tool_name = call.name;
    pending.partial_arguments =
        call.arguments.is_object() ? call.arguments : nlohmann::json::object();
    pending.action = ConfigurationLifecycleAction::None;
    pending.created_at_unix_sec = confirmationNowUnixSec();
    ToolArgumentFieldSpec missing_field;
    missing_field.name = field_name.empty() ? "class_name" : field_name;
    missing_field.type = "string";
    missing_field.description =
        kind == PendingDisambiguationKind::Class
            ? "Registered component class (reply with an exact name from the list)"
            : "Resolved component long_name (reply with an exact name from the list)";
    pending.missing_fields = {missing_field};
    pending.disambiguation_kind = kind;
    pending.disambiguation_field = missing_field.name;
    pending.disambiguation_candidates =
        disambiguation.is_object() ? disambiguation.value("candidates", nlohmann::json::array())
                                   : nlohmann::json::array();
    pending.class_disambiguation_candidates =
        kind == PendingDisambiguationKind::Class ? pending.disambiguation_candidates
                                                 : nlohmann::json::array();
    if(call.name == "add_component")
        pending.requested_repeat_count = primaryQuantityOr(state);
    m_store.setPendingToolArguments(state.session_id, pending);

    const std::string prompt = formatClarificationMessage(
        disambiguation.is_object() ? disambiguation : nlohmann::json::object(),
        include_candidate_list);
    LLMMessage assistant_msg;
    assistant_msg.role = LLMMessage::Role::Assistant;
    assistant_msg.content = prompt;
    m_store.appendMessage(state.session_id, assistant_msg);

    final.ok = true;
    final.needs_argument_clarification = true;
    final.needs_tool_disambiguation = true;
    final.needs_entity_clarification = true;
    final.clarification_candidates = disambiguation.value("candidates", nlohmann::json::array());
    final.text = prompt;
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    m_store.persistToDisk(state.session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::returnClarificationViaAskUser(
    ConversationState& state, const std::string& trace_id, const LLMToolCall& call,
    PendingDisambiguationKind kind, const std::string& field_name,
    const nlohmann::json& disambiguation)
{
    // Short prompt: dock renders user_choice_options once (no duplicate numbered dump).
    LLMFinalResponse final = returnDisambiguationRequest(state, trace_id, call, kind, field_name,
                                                         disambiguation, false);
    final.needs_argument_clarification = false;
    final.needs_tool_disambiguation = false;
    final.needs_entity_clarification = false;

    const std::string prompt = final.text;
    PendingUserQuestion pq;
    pq.question_id = call.id.empty() ? trace_id : call.id;
    pq.prompt = prompt;
    pq.allow_free_text = true;
    const nlohmann::json candidates = disambiguation.value("candidates", nlohmann::json::array());
    if(candidates.is_array())
    {
        for(const auto& c : candidates)
        {
            if(kind == PendingDisambiguationKind::Class && c.is_object()
               && c.contains("class_name"))
                pq.choices.push_back(c["class_name"].get<std::string>());
            else if(c.is_object() && c.contains("long_name"))
                pq.choices.push_back(c["long_name"].get<std::string>());
            else if(c.is_string())
                pq.choices.push_back(c.get<std::string>());
        }
    }
    state.pending_user_question = std::move(pq);
    setWorkflowPhase(state, LLMWorkflowPhase::AwaitingUserInput, trace_id);
    final.awaiting_user_input = true;
    final.pending_question_id = state.pending_user_question->question_id;
    final.user_choice_options = state.pending_user_question->choices;
    final.text = prompt;
    GetAuditLog().append("clarify_via_ask_user",
                         {{"tool_name", call.name}, {"question_id", final.pending_question_id}},
                         trace_id, state.session_id);
    m_store.persistToDisk(state.session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::routeClarificationOrDisambiguation(
    ConversationState& state, const std::string& trace_id, const LLMToolCall& call,
    PendingDisambiguationKind kind, const std::string& field_name,
    const nlohmann::json& disambiguation)
{
    nlohmann::json enriched =
        disambiguation.is_object() ? disambiguation : nlohmann::json::object();
    if(kind == PendingDisambiguationKind::Class && state.session_graph.last_add
       && !state.session_graph.last_add->class_name.empty())
    {
        enrichClassDisambiguationWithLastAdd(enriched,
                                             state.session_graph.last_add->class_name);
    }
    if(clarifyInLoopEnabled())
        return returnClarificationViaAskUser(state, trace_id, call, kind, field_name, enriched);
    return returnDisambiguationRequest(state, trace_id, call, kind, field_name, enriched);
}

LLMFinalResponse ULLMAgentOrchestrator::returnArgumentRequest(ConversationState& state,
                                                              const std::string& trace_id,
                                                              const PendingToolArguments& pending_in,
                                                              RDK::UApplication* app)
{
    LLMFinalResponse final;
    PendingToolArguments pending = pending_in;
    if(pending.created_at_unix_sec == 0)
        pending.created_at_unix_sec = confirmationNowUnixSec();
    m_store.setPendingToolArguments(state.session_id, pending);

    const std::string prompt =
        formatArgumentRequestPrompt(pending.tool_name, pending.action, pending.missing_fields, app);

    LLMMessage assistant_msg;
    assistant_msg.role = LLMMessage::Role::Assistant;
    assistant_msg.content = prompt;
    m_store.appendMessage(state.session_id, assistant_msg);

    final.ok = true;
    final.needs_argument_clarification = true;
    final.text = prompt;
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    m_store.persistToDisk(state.session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::invokeLifecycleToolDirect(const std::string& session_id,
                                                                const std::string& trace_id,
                                                                const std::string& tool_name,
                                                                const nlohmann::json& arguments,
                                                                const LLMSessionContext& session,
                                                                const std::string& user_text_hint)
{
    LLMFinalResponse final;
    ConversationState& state = m_store.getOrCreate(session_id);

    RDK::UApplication* app = nullptr;
    std::string user_lang = "en";
    if(LLMServices::instance().isInitialized())
    {
        app = LLMServices::instance().domain().application();
        user_lang = resolveResponseLanguage(
            LLMServices::instance().settings().runtime().preferred_response_language, "en");
    }

    const LLMGuiContextSnapshot gui = guiSnapshotForWrite(state, LLMGuiContextSnapshot{});
    RecordedToolInvokeDeps deps{m_registry, m_gateway, m_store, {}, m_system_log_reader.get()};
    deps.write_exec = [this](ConversationState& st, WriteToolExecutionRequest& wreq) {
        return executeWriteWithPreviewAndVerify(*this, st, wreq);
    };

    RecordedToolInvokeRequest rreq;
    rreq.session_id = session_id;
    rreq.trace_id = trace_id;
    rreq.tool_name = tool_name;
    rreq.arguments = arguments.is_object() ? arguments : nlohmann::json::object();
    rreq.session = session;
    rreq.session.session_id = session_id;
    rreq.gui = gui;
    rreq.user_text_hint = user_text_hint;
    rreq.user_lang = user_lang;
    rreq.idempotency_action_id = "lifecycle_direct";

    RecordedToolInvokeResult recorded = recordedToolInvoke(state, deps, rreq);
    const ToolGatewayResult& tr = recorded.gateway;

    ToolInvokeRequest invoke;
    invoke.trace_id = trace_id;
    invoke.tool_name = tool_name;
    invoke.arguments = rreq.arguments;
    invoke.session = rreq.session;
    invoke.user_text_hint = user_text_hint;

    if(recorded.needs_hitl)
    {
        setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, trace_id);
        PendingConfirmation pending;
        pending.confirmation_id = tr.confirmation_id;
        pending.created_at_unix_sec = confirmationNowUnixSec();
        pending.request = invoke;
        pending.request.confirmed = true;
        pending.tool_call_id = recorded.tool_call_id;
        m_store.setPending(session_id, pending);
        final.ok = true;
        final.pending_confirmation = true;
        final.pending_confirmation_id = tr.confirmation_id;
        final.action_preview_text = recorded.preview_text.empty()
                                        ? formatActionIntentPreview(tool_name, rreq.arguments, gui,
                                                                    session, user_lang)
                                        : recorded.preview_text;
        final.text = recorded.outcome_text.empty()
                         ? formatHitlConfirmationText(final.action_preview_text, user_lang)
                         : recorded.outcome_text;
        GetAuditLog().append("escalation_to_hitl",
                             {{"tool_name", tool_name},
                              {"confirmation_id", tr.confirmation_id},
                              {"tool_call_id", recorded.tool_call_id}},
                             trace_id, session_id);
        m_store.persistToDisk(session_id);
        return final;
    }

    if(tr.ok && tool_name == "set_active_channel" && rreq.arguments.contains("channel_index"))
        invoke.session.active_channel_index = rreq.arguments["channel_index"].get<int>();

    nlohmann::json disambiguation;
    if(extractToolDisambiguationPayload(tr, disambiguation)
       && disambiguation.is_object() && disambiguation.value("kind", "") == "class")
    {
        LLMToolCall call;
        call.name = tool_name;
        call.arguments = rreq.arguments;
        return routeClarificationOrDisambiguation(state, trace_id, call,
                                                PendingDisambiguationKind::Class, "class_name",
                                                disambiguation);
    }
    if(extractToolDisambiguationPayload(tr, disambiguation)
       && disambiguation.is_object() && disambiguation.value("kind", "") == "component")
    {
        LLMToolCall call;
        call.name = tool_name;
        call.arguments = rreq.arguments;
        return routeClarificationOrDisambiguation(
            state, trace_id, call, PendingDisambiguationKind::Component,
            disambiguation.value("field", "long_name"), disambiguation);
    }

    if(!tr.ok && tool_name == "add_component"
       && (tr.error_code == "CLASS_NOT_REGISTERED" || tr.error_code == "CLASS_AMBIGUOUS"))
    {
        LLMFinalResponse response;
        response.ok = false;
        response.error = tr.message;
        response.text = tr.message;
        if(state.pending_tool_arguments
           && state.pending_tool_arguments->class_disambiguation_candidates.is_array()
           && !state.pending_tool_arguments->class_disambiguation_candidates.empty())
        {
            nlohmann::json payload;
            payload["kind"] = "class";
            payload["candidates"] = state.pending_tool_arguments->class_disambiguation_candidates;
            response.text = tr.message + "\n\n" + formatClarificationMessage(payload);
            response.needs_tool_disambiguation = true;
            response.needs_entity_clarification = true;
            response.clarification_candidates = payload["candidates"];
        }
        m_store.persistToDisk(session_id);
        return response;
    }

    const std::vector<ToolArgumentFieldSpec> direct_missing =
        findMissingArgumentsForTool(tool_name, rreq.arguments, app, m_registry);
    if(shouldPromptForMissingToolArguments(tool_name, tr, direct_missing))
    {
        PendingToolArguments pending;
        pending.tool_name = tool_name;
        pending.action = lifecycleActionFromToolName(tool_name);
        pending.partial_arguments = rreq.arguments;
        pending.missing_fields = direct_missing;
        if(pending.missing_fields.empty() && pending.action != ConfigurationLifecycleAction::None)
            pending.missing_fields = argumentFieldsForLifecycle(pending.action);
        GetAuditLog().append("lifecycle_args_requested",
                             {{"tool_name", tool_name}, {"error_code", tr.error_code}}, trace_id,
                             session_id);
        return returnArgumentRequest(state, trace_id, pending, app);
    }
    if(toolInvokeNeedsArgumentClarification(tool_name, tr))
    {
        final.ok = false;
        final.error = tr.message;
        final.text = tr.message.empty() ? "Tool invocation failed: " + tool_name : tr.message;
        m_store.persistToDisk(session_id);
        return final;
    }

    final.ok = tr.ok;
    final.text = recorded.outcome_text;
    if(final.text.empty())
    {
        if(tool_name == "add_component" || isNetGraphWriteTool(tool_name))
            final.text = formatWriteToolUserMessage(tool_name, tr);
        else
            final.text = formatLifecycleToolUserMessage(tool_name, tr);
    }
    if(!tr.ok && !tr.message.empty())
        final.error = tr.message;
    setWorkflowPhase(state, LLMWorkflowPhase::Completed, trace_id);
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    m_store.persistToDisk(session_id);
    return final;
}

void ULLMAgentOrchestrator::rejectPending(const std::string& session_id)
{
    ConversationState& state = m_store.getOrCreate(session_id);
    if(state.pending_plan)
    {
        GetAuditLog().append("workflow_compensation_skipped",
                             {{"plan_id", state.pending_plan->plan_id},
                              {"reason", "user_rejected_confirmation"}},
                             "", session_id);
        state.pending_plan.reset();
    }
    m_store.clearPending(session_id);
    m_store.clearPendingToolArguments(session_id);
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, "");
    GetAuditLog().append("confirmation_rejected", {}, "", session_id);
}

void ULLMAgentOrchestrator::seedSessionContext(const std::string& session_id,
                                               const LLMSessionContext& session,
                                               const LLMGuiContextSnapshot& gui)
{
    ConversationState& state = m_store.getOrCreate(session_id);
    if(state.session_context_seeded)
        return;

    std::ostringstream bootstrap;
    bootstrap << "## Session bootstrap\n"
              << "- session_id: " << session_id << "\n"
              << "- project_loaded: " << (session.project_loaded ? "true" : "false") << "\n"
              << "- active_channel: " << session.active_channel_index << "\n"
              << "- write_enabled: " << (session.llm_write_enabled ? "true" : "false") << "\n"
              << "- auto_apply: " << (session.auto_apply_writes ? "true" : "false") << "\n"
              << "- autonomous: "
              << (session.autonomous_mode == LLMAutonomousMode::Strict       ? "strict"
                  : session.autonomous_mode == LLMAutonomousMode::SemiAuto ? "semi_auto"
                                                                           : "off")
              << "\n"
              << "- user: " << session.user_name << " (id=" << session.user_id << ")\n"
              << "Use read tools to inspect the graph before mutating.\n";

    LLMMessage boot;
    boot.role = LLMMessage::Role::System;
    boot.content = bootstrap.str();
    m_store.appendMessage(session_id, boot);

    const char* bootstrap_env = std::getenv("NMSDK_LLM_SESSION_BOOTSTRAP");
    if(bootstrap_env && bootstrap_env[0] == '1' && session.project_loaded)
    {
        ToolInvokeRequest snap_req;
        snap_req.trace_id = "session-bootstrap";
        snap_req.tool_name = "get_net_snapshot";
        snap_req.arguments = {{"channel_index", session.active_channel_index},
                              {"max_components", 20}};
        snap_req.session = session;
        const ToolGatewayResult snap_res = m_gateway.invoke(snap_req);
        if(snap_res.ok)
        {
            LLMMessage tool_msg;
            tool_msg.role = LLMMessage::Role::Tool;
            tool_msg.tool_call_id = "bootstrap-snapshot";
            tool_msg.tool_name = "get_net_snapshot";
            tool_msg.content = snap_res.result.dump();
            m_store.appendMessage(session_id, tool_msg);
        }
    }

    state.session_context_seeded = true;
    if(!gui.focused_component_long_name.empty() || !gui.focused_class_name.empty())
        state.last_gui_context = gui;
    m_store.persistToDisk(session_id);
}

const char* ULLMAgentOrchestrator::sessionBusyErrorMessage()
{
    return kSessionBusyError;
}

bool ULLMAgentOrchestrator::tryAcquireSessionBusy(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(m_session_busy_mu);
    if(m_session_busy[session_id])
        return false;
    m_session_busy[session_id] = true;
    return true;
}

void ULLMAgentOrchestrator::releaseSessionBusy(const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(m_session_busy_mu);
    m_session_busy[session_id] = false;
}

bool ULLMAgentOrchestrator::isSessionBusy(const std::string& session_id) const
{
    std::lock_guard<std::mutex> lock(m_session_busy_mu);
    const auto it = m_session_busy.find(session_id);
    return it != m_session_busy.end() && it->second;
}

bool ULLMAgentOrchestrator::tryResumeSession(const std::string& session_id)
{
    if(isSessionBusy(session_id))
        return false;
    if(!m_store.loadFromDisk(session_id))
        return false;
    {
        std::lock_guard<std::mutex> cancel_lock(m_cancel_mu);
        m_cancelled_sessions.erase(session_id);
    }
    return true;
}

void ULLMAgentOrchestrator::discardSession(const std::string& session_id)
{
    if(isSessionBusy(session_id))
        return;

    cancelSession(session_id);
    rejectPending(session_id);
    m_store.removeSession(session_id);
    std::lock_guard<std::mutex> lock(m_session_busy_mu);
    m_session_busy.erase(session_id);
    std::lock_guard<std::mutex> cancel_lock(m_cancel_mu);
    m_cancelled_sessions.erase(session_id);
}

} // namespace RDK::LLM
