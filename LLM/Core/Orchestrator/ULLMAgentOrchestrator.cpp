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
#include "../Settings/ULLMProviderAuth.h"
#include "ULLMExecutionPlan.h"
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
    ToolFilter filter;
    filter.intent = intent;
    filter.include_write = (intent == LLMIntentKind::Mutate) && session.llm_write_enabled;

    std::vector<LLMMessage> provider_messages = state.messages;
    if(intent == LLMIntentKind::Plan)
    {
        LLMMessage plan_hint;
        plan_hint.role = LLMMessage::Role::System;
        plan_hint.content =
            "Plan-only mode: use read tools to inspect state, then reply with a numbered execution "
            "plan. Do not mutate the project until the user confirms.";
        provider_messages.insert(provider_messages.begin(), plan_hint);
        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);
    }

    LLMCompletionOptions opts;
    const bool provider_tools = m_provider.capabilities().supports_tool_calling;
    if(provider_tools)
        opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
    else
        opts.tools_for_api.clear();

    int tool_invocations = 0;
    const int max_tool_invocations = defaultPolicyLimits().max_tool_invocations_per_message;

    const bool is_cloud_profile = req.provider_profile.is_cloud;

    for(int round = 0; round < kMaxRounds && !m_cancelled; ++round)
    {
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
                final.error += " (check: ollama serve, model pulled, Ollama 0.3+ for tools)";
            }
            setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            return final;
        }

        if(completion.tool_calls.empty())
        {
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
            for(const LLMToolCall& call : completion.tool_calls)
            {
                const auto [call_copy, tr] = invokeOne(call);
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
                    final.text = "Confirmation required for: " + call_copy.name;
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
            }
        }

        if(provider_tools)
            opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
    }

    setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
    final.text = "Stopped: maximum tool rounds reached.";
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
    setWorkflowPhase(state, LLMWorkflowPhase::Idle, "");
    GetAuditLog().append("confirmation_rejected", {}, "", session_id);
}

} // namespace RDK::LLM
