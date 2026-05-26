#include "ULLMAgentOrchestrator.h"

#include <algorithm>
#include <functional>
#include <future>
#include <sstream>

#include "../LlmModuleInit.h"
#include "../Session/ULLMConfirmationExpiry.h"
#include "../LlmPublicApi.h"
#include "../Policy/ULLMPolicyEngine.h"
#include "../Providers/UOllamaChatTemplate.h"
#include "../Providers/UOllamaModelInfo.h"
#include "../Settings/ULLMProviderAuth.h"
#include "../Settings/ULLMResponseLanguage.h"
#include "../Settings/ULLMUserMessages.h"
#include "ULLMConfigurationLifecycle.h"
#include "ULLMLifecycleArgumentGate.h"
#include "ULLMToolFilterBuilder.h"
#include "ULLMEmbeddedToolCalls.h"
#include "ULLMExecutionPlan.h"
#include "ULLMAgentManifestBuilder.h"
#include "ULLMPlanExecutor.h"

namespace RDK::LLM {

namespace {

std::string formatClarificationMessage(const nlohmann::json& candidates)
{
    std::ostringstream oss;
    oss << "Multiple components match. Please specify which one:\n";
    int index = 1;
    for(const auto& c : candidates)
    {
        oss << index++ << ". " << c.value("long_name", "") << " ("
            << c.value("class_name", "") << " / " << c.value("short_name", "") << ")\n";
    }
    return oss.str();
}

bool extractAmbiguousFindComponent(const ToolGatewayResult& tr, nlohmann::json& candidates_out)
{
    if(!tr.ok || !tr.result.value("ambiguous", false))
        return false;
    candidates_out = tr.result.value("candidates", nlohmann::json::array());
    return candidates_out.size() > 1;
}

} // namespace

ULLMAgentOrchestrator::ULLMAgentOrchestrator(ILLMProvider& provider, ULLMToolRegistry& registry,
                                             ULLMToolGateway& gateway, ULLMConversationStore& store)
    : m_provider(provider)
    , m_registry(registry)
    , m_gateway(gateway)
    , m_store(store)
{
}

void ULLMAgentOrchestrator::cancel()
{
    m_cancelled = true;
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
    m_cancelled = false;
    LLMFinalResponse final;
    {
        std::lock_guard<std::mutex> lock(m_session_busy_mu);
        if(m_session_busy[req.session_id])
        {
            final.ok = false;
            final.error = "Session busy: wait for the current request to finish.";
            return final;
        }
        m_session_busy[req.session_id] = true;
    }
    auto clear_busy = [this, sid = req.session_id]() {
        std::lock_guard<std::mutex> lock(m_session_busy_mu);
        m_session_busy[sid] = false;
    };
    struct BusyGuard {
        std::function<void()> fn;
        ~BusyGuard() { fn(); }
    } busy_guard{clear_busy};

    ConversationState& state = m_store.getOrCreate(req.session_id);
    state.session_id = req.session_id;

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

    LLMSessionContext session = req.session;
    if(LLMServices::instance().isInitialized())
    {
        session.llm_write_enabled = LLMServices::instance().settings().runtime().llm_write_enabled;
        session.allow_cloud_llm = LLMServices::instance().settings().runtime().allow_cloud_providers;

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

    const IntentParseResult intent_result =
        m_intent.parseWithOptionalLlm(&m_provider, req.user_text);
    const LLMIntentKind intent = intent_result.kind;
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
    if(intent == LLMIntentKind::Mutate && !session.llm_write_enabled)
    {
        final.ok = false;
        final.error =
            "LLM write tools are disabled. Enable \"Allow LLM write tools\" in AI Assistant Settings.";
        setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
        m_store.persistToDisk(req.session_id);
        return final;
    }

    const ConfigurationLifecycleAction lifecycle_action =
        detectConfigurationLifecycleAction(req.user_text);

    RDK::UApplication* app = nullptr;
    if(LLMServices::instance().isInitialized())
        app = LLMServices::instance().domain().application();

    if(state.pending_tool_arguments)
    {
        PendingToolArguments pending = *state.pending_tool_arguments;
        if(lifecycle_action != ConfigurationLifecycleAction::None
           && lifecycle_action != pending.action)
            m_store.clearPendingToolArguments(req.session_id);
        else
        {
            nlohmann::json merged =
                mergeArgumentsFromUserText(pending, req.user_text, app);
            const std::vector<ToolArgumentFieldSpec> still_missing =
                findMissingLifecycleFields(pending.tool_name, merged, app);
            if(!still_missing.empty())
            {
                pending.partial_arguments = std::move(merged);
                pending.missing_fields = still_missing;
                return returnArgumentRequest(state, req.trace_id, pending, app);
            }
            m_store.clearPendingToolArguments(req.session_id);
            GetAuditLog().append("lifecycle_args_resolved",
                                 {{"tool_name", pending.tool_name}}, req.trace_id,
                                 req.session_id);
            return invokeLifecycleToolDirect(req.session_id, req.trace_id, pending.tool_name,
                                             merged, session);
        }
    }

    ToolFilter filter =
        buildToolFilter(intent, session.llm_write_enabled, lifecycle_action);

    std::vector<LLMMessage> provider_messages = state.messages;
    const bool strict_plan_schema =
        intent == LLMIntentKind::Plan && providerSupportsStrictPlanSchema(req.provider_profile)
        && m_provider.capabilities().supports_strict_json_schema;
    if(intent == LLMIntentKind::Plan)
    {
        LLMMessage plan_hint;
        plan_hint.role = LLMMessage::Role::System;
        plan_hint.content = strict_plan_schema
                                ? "Plan-only mode: use read tools to inspect state, then respond "
                                  "with JSON matching the execution_plan schema (no markdown)."
                                : "Plan-only mode: use read tools to inspect state, then reply with "
                                  "a JSON execution plan in a ```json code block. Do not mutate until "
                                  "the user confirms.";
        provider_messages.insert(provider_messages.begin(), plan_hint);
        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);
    }

    const bool provider_tools = m_provider.capabilities().supports_tool_calling;

    if(intent == LLMIntentKind::Query && provider_tools)
    {
        LLMMessage query_hint;
        query_hint.role = LLMMessage::Role::System;
        query_hint.content =
            "Use search_project_docs(scope=docs) and describe_class. Cite source_id and path. "
            "Do not call write tools.";
        provider_messages.insert(provider_messages.begin(), query_hint);
    }

    if(lifecycle_action != ConfigurationLifecycleAction::None && provider_tools)
    {
        LLMMessage lifecycle_hint;
        lifecycle_hint.role = LLMMessage::Role::System;
        lifecycle_hint.content =
            configurationLifecycleSystemHint(lifecycle_action, session.project_loaded);
        provider_messages.insert(provider_messages.begin(), lifecycle_hint);
    }
    if(provider_tools)
    {
        LLMMessage manifest;
        manifest.role = LLMMessage::Role::System;
        manifest.content = buildAgentManifest(m_registry, filter);
        provider_messages.insert(provider_messages.begin(), manifest);
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
    if(strict_plan_schema)
        opts.response_format = executionPlanOpenAiResponseFormat();
    const std::string user_lang = opts.response_language;

    int tool_invocations = 0;
    const int max_tool_invocations = defaultPolicyLimits().max_tool_invocations_per_message;
    bool recovery_used = false;

    const bool is_cloud_profile = req.provider_profile.is_cloud;
    const int max_rounds = kMaxRounds;

    for(int round = 0; round < max_rounds && !m_cancelled; ++round)
    {
        provider_messages = state.messages;

        if(lifecycle_action != ConfigurationLifecycleAction::None && round >= 1)
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
            m_provider.chatStream(
                provider_messages, opts,
                [&](const std::string& token) {
                    if(!m_cancelled && stream->on_token)
                        stream->on_token(token);
                },
                [&](LLMCompletionResult r) { completion = std::move(r); });
        }
        else
        {
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
            setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            return final;
        }

        if(completion.tool_calls.empty() && intent == LLMIntentKind::Mutate && filter.include_write
           && provider_tools)
        {
            std::vector<LLMToolCall> embedded =
                tryExtractEmbeddedToolCalls(completion.text, m_registry);
            if(!embedded.empty())
                completion.tool_calls = std::move(embedded);
        }

        if(completion.tool_calls.empty())
        {
            if(intent == LLMIntentKind::Mutate && filter.include_write && provider_tools
               && tool_invocations == 0)
            {
                if(!recovery_used)
                {
                    recovery_used = true;
                    LLMMessage recovery;
                    recovery.role = LLMMessage::Role::System;
                    recovery.content =
                        "Mutate request detected. Call exactly one suitable tool. "
                        "If no tool can satisfy the request, reply exactly: NO_SUITABLE_TOOL.";
                    m_store.appendMessage(req.session_id, recovery);
                    continue;
                }
                final.no_suitable_tool = true;
                final.text = formatUserMessage("error.no_suitable_tool", user_lang);
                setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
                setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
                m_store.persistToDisk(req.session_id);
                return final;
            }

            LLMMessage assistant;
            assistant.role = LLMMessage::Role::Assistant;
            assistant.content = completion.text;
            m_store.appendMessage(req.session_id, assistant);
            final.text = completion.text;
            if(intent == LLMIntentKind::Plan)
            {
                if(auto plan = parseExecutionPlanFromAssistantText(completion.text))
                {
                    ULLMPolicyEngine policy;
                    const PolicyDecision plan_pol = policy.checkPlan(*plan, session, m_registry);
                    if(plan_pol.allowed)
                    {
                        state.pending_plan = *plan;
                        final.pending_plan_execution = true;
                        final.pending_plan_id = plan->plan_id;
                        final.text = formatExecutionPlanPreview(*plan) +
                                     "\n\n[Plan ready — confirm execution in the assistant panel.]";
                        setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, req.trace_id);
                        m_store.persistToDisk(req.session_id);
                        return final;
                    }
                    final.text += "\n\n(Plan rejected by policy: " + plan_pol.deny_message + ")";
                }
            }
            setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            m_store.persistToDisk(req.session_id);
            return final;
        }

        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);

        LLMMessage assistant_tools;
        assistant_tools.role = LLMMessage::Role::Assistant;
        assistant_tools.content = completion.text;
        assistant_tools.assistant_tool_calls = completion.tool_calls;
        m_store.appendMessage(req.session_id, assistant_tools);

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
            ToolInvokeRequest invoke;
            invoke.trace_id = req.trace_id;
            invoke.tool_name = call.name;
            invoke.arguments = call.arguments;
            invoke.session = session;
            return {call, m_gateway.invoke(invoke)};
        };

        const bool all_read = std::all_of(
            completion.tool_calls.begin(), completion.tool_calls.end(),
            [&](const LLMToolCall& call) {
                const LLMToolDefinition* def = m_registry.find(call.name);
                return def && def->kind == LLMToolKind::Read;
            });

        if(all_read && completion.tool_calls.size() > 1)
        {
            std::vector<std::future<std::pair<LLMToolCall, ToolGatewayResult>>> futures;
            futures.reserve(completion.tool_calls.size());
            for(const LLMToolCall& call : completion.tool_calls)
                futures.push_back(std::async(std::launch::async, invokeOne, call));
            for(auto& fut : futures)
            {
                auto [call, tr] = fut.get();
                nlohmann::json ambiguous;
                if(extractAmbiguousFindComponent(tr, ambiguous))
                {
                    final.needs_entity_clarification = true;
                    final.clarification_candidates = ambiguous;
                    final.text = formatClarificationMessage(ambiguous);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }
                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call.id;
                tool_msg.tool_name = call.name;
                tool_msg.content = tr.result.dump();
                m_store.appendMessage(req.session_id, tool_msg);
            }
        }
        else
        {
            std::optional<std::pair<std::string, ToolGatewayResult>> lifecycle_write_done;
            for(const LLMToolCall& call : completion.tool_calls)
            {
                const auto [call_copy, tr] = invokeOne(call);
                if(toolInvokeNeedsArgumentClarification(call_copy.name, tr))
                {
                    PendingToolArguments pending;
                    pending.tool_name = call_copy.name;
                    pending.action = lifecycleActionFromToolName(call_copy.name);
                    pending.partial_arguments = call_copy.arguments;
                    pending.missing_fields =
                        findMissingLifecycleFields(call_copy.name, call_copy.arguments, app);
                    if(pending.missing_fields.empty()
                       && pending.action != ConfigurationLifecycleAction::None)
                        pending.missing_fields = argumentFieldsForLifecycle(pending.action);
                    GetAuditLog().append("lifecycle_args_requested",
                                         {{"tool_name", call_copy.name},
                                          {"error_code", tr.error_code}},
                                         req.trace_id, req.session_id);
                    return returnArgumentRequest(state, req.trace_id, pending, app);
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
                    final.text = formatUserMessage("confirmation.required", user_lang,
                                                   {{"tool_name", call_copy.name}});
                    m_store.persistToDisk(req.session_id);
                    return final;
                }

                nlohmann::json ambiguous;
                if(extractAmbiguousFindComponent(tr, ambiguous))
                {
                    final.needs_entity_clarification = true;
                    final.clarification_candidates = ambiguous;
                    final.text = formatClarificationMessage(ambiguous);
                    m_store.persistToDisk(req.session_id);
                    return final;
                }

                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call_copy.id;
                tool_msg.tool_name = call_copy.name;
                tool_msg.content = tr.result.dump();
                m_store.appendMessage(req.session_id, tool_msg);

                if(lifecycle_action != ConfigurationLifecycleAction::None
                   && isLifecycleWriteToolName(call_copy.name))
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
            opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
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
    m_store.persistToDisk(req.session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::confirmPending(const std::string& session_id,
                                                      const std::string& confirmation_id)
{
    LLMFinalResponse final;
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
    ToolInvokeRequest req = state.pending->request;
    req.confirmed = true;
    const ToolGatewayResult tr = m_gateway.invoke(req);
    m_store.clearPending(session_id);
    final.ok = tr.ok;
    final.text = tr.ok ? "Change applied." : tr.message;
    if(!tr.ok)
        final.error = tr.message;
    m_store.persistToDisk(session_id);
    return final;
}

LLMFinalResponse ULLMAgentOrchestrator::confirmPlanExecution(const std::string& session_id,
                                                             const std::string& trace_id,
                                                             const LLMSessionContext& session)
{
    LLMFinalResponse final;
    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan)
    {
        final.ok = false;
        final.error = "No pending execution plan";
        return final;
    }

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
    const PlanExecutionResult exec =
        executor.execute(plan, session, trace_id, planExecuteWithCheckpointOnFailure());

    if(exec.ok)
    {
        state.pending_plan.reset();
        final.ok = true;
        final.text = exec.summary;
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
    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan || !state.pending_plan->paused)
    {
        final.ok = false;
        final.error = "No paused plan to resume";
        return final;
    }

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
    const PlanExecutionResult exec = executor.execute(plan, session, trace_id, planExecuteResume());

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
    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending_plan)
    {
        final.ok = false;
        final.error = "No plan to rollback";
        return final;
    }

    ULLMPlanExecutor executor(m_registry, m_gateway);
    std::string note;
    const int applied =
        executor.compensateCompletedWrites(*state.pending_plan, session, trace_id, note);
    GetAuditLog().append("plan_rollback",
                         {{"plan_id", state.pending_plan->plan_id}, {"applied", applied}, {"note", note}},
                         trace_id, session_id);

    state.pending_plan.reset();
    final.ok = applied > 0 || note.empty();
    final.text = note.empty() ? "Plan discarded (no completed write steps to rollback)."
                              : "Plan rolled back. " + note;
    m_store.persistToDisk(session_id);
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, trace_id);
    return final;
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
                                                                const LLMSessionContext& session)
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

    ToolInvokeRequest invoke;
    invoke.trace_id = trace_id;
    invoke.tool_name = tool_name;
    invoke.arguments = arguments;
    invoke.session = session;
    invoke.session.session_id = session_id;

    const ToolGatewayResult tr = m_gateway.invoke(invoke);

    if(toolInvokeNeedsArgumentClarification(tool_name, tr))
    {
        PendingToolArguments pending;
        pending.tool_name = tool_name;
        pending.action = lifecycleActionFromToolName(tool_name);
        pending.partial_arguments = arguments;
        pending.missing_fields = findMissingLifecycleFields(tool_name, arguments, app);
        if(pending.missing_fields.empty() && pending.action != ConfigurationLifecycleAction::None)
            pending.missing_fields = argumentFieldsForLifecycle(pending.action);
        GetAuditLog().append("lifecycle_args_requested",
                             {{"tool_name", tool_name}, {"error_code", tr.error_code}}, trace_id,
                             session_id);
        return returnArgumentRequest(state, trace_id, pending, app);
    }

    if(tr.pending_confirmation)
    {
        setWorkflowPhase(state, LLMWorkflowPhase::AwaitingConfirmation, trace_id);
        PendingConfirmation pending;
        pending.confirmation_id = tr.confirmation_id;
        pending.created_at_unix_sec = confirmationNowUnixSec();
        pending.request = invoke;
        pending.request.confirmed = true;
        m_store.setPending(session_id, pending);
        final.ok = true;
        final.pending_confirmation = true;
        final.pending_confirmation_id = tr.confirmation_id;
        final.text = formatUserMessage("confirmation.required", user_lang, {{"tool_name", tool_name}});
        m_store.persistToDisk(session_id);
        return final;
    }

    final.ok = tr.ok;
    final.text = formatLifecycleToolUserMessage(tool_name, tr);
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

void ULLMAgentOrchestrator::discardSession(const std::string& session_id)
{
    cancel();
    rejectPending(session_id);
    m_store.removeSession(session_id);
    std::lock_guard<std::mutex> lock(m_session_busy_mu);
    m_session_busy.erase(session_id);
}

} // namespace RDK::LLM
