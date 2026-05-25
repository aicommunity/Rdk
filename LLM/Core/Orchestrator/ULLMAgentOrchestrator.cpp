#include "ULLMAgentOrchestrator.h"

#include <algorithm>
#include <future>
#include <sstream>

#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Settings/ULLMProviderAuth.h"

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

LLMFinalResponse ULLMAgentOrchestrator::handleUserMessage(const LLMRequestEnvelope& req)
{
    m_cancelled = false;
    LLMFinalResponse final;
    ConversationState& state = m_store.getOrCreate(req.session_id);
    state.session_id = req.session_id;

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

    const LLMIntentKind intent = m_intent.parse(req.user_text);
    ToolFilter filter;
    filter.intent = intent;
    filter.include_write = (intent == LLMIntentKind::Mutate) && session.llm_write_enabled;

    LLMCompletionOptions opts;
    const bool provider_tools = m_provider.capabilities().supports_tool_calling;
    if(provider_tools)
        opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
    else
        opts.tools_for_api.clear();

    int tool_invocations = 0;
    const int max_tool_invocations = defaultPolicyLimits().max_tool_invocations_per_message;

    for(int round = 0; round < kMaxRounds && !m_cancelled; ++round)
    {
        LLMCompletionResult completion = m_provider.chat(state.messages, opts);
        if(!completion.ok)
        {
            final.ok = false;
            final.error = completion.error_message;
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
            setWorkflowPhase(state, LLMWorkflowPhase::Completed, req.trace_id);
            setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            m_store.persistToDisk(req.session_id);
            return final;
        }

        setWorkflowPhase(state, LLMWorkflowPhase::Executing, req.trace_id);

        LLMMessage assistant_tools;
        assistant_tools.role = LLMMessage::Role::Assistant;
        assistant_tools.content = completion.text;
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
                    pending.request = ToolInvokeRequest{};
                    pending.request.trace_id = req.trace_id;
                    pending.request.tool_name = call_copy.name;
                    pending.request.arguments = call_copy.arguments;
                    pending.request.session = session;
                    pending.request.confirmed = true;
                    m_store.setPending(req.session_id, pending);
                    final.pending_confirmation = true;
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

void ULLMAgentOrchestrator::confirmPending(const std::string& session_id,
                                           const std::string& confirmation_id)
{
    ConversationState& state = m_store.getOrCreate(session_id);
    if(!state.pending || state.pending->confirmation_id != confirmation_id)
        return;
    ToolInvokeRequest req = state.pending->request;
    req.confirmed = true;
    m_gateway.invoke(req);
    m_store.clearPending(session_id);
}

void ULLMAgentOrchestrator::rejectPending(const std::string& session_id)
{
    m_store.clearPending(session_id);
    GetAuditLog().append("confirmation_rejected", {}, "", session_id);
}

} // namespace RDK::LLM
