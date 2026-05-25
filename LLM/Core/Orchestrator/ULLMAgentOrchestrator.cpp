#include "ULLMAgentOrchestrator.h"

#include "../LlmModuleInit.h"

namespace RDK::LLM {

ULLMAgentOrchestrator::ULLMAgentOrchestrator(ILLMProvider& provider, ULLMToolRegistry& registry,
                                             ULLMToolGateway& gateway, ULLMConversationStore& store)
    : m_provider(provider)
    , m_registry(registry)
    , m_gateway(gateway)
    , m_store(store)
{
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

    const LLMIntentKind intent = m_intent.parse(req.user_text);
    ToolFilter filter;
    filter.intent = intent;
    filter.include_write = (intent == LLMIntentKind::Mutate);

    LLMCompletionOptions opts;
    opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);

    for(int round = 0; round < kMaxRounds && !m_cancelled; ++round)
    {
        LLMCompletionResult completion = m_provider.chat(state.messages, opts);
        if(!completion.ok)
        {
            final.ok = false;
            final.error = completion.error_message;
            return final;
        }

        if(completion.tool_calls.empty())
        {
            LLMMessage assistant;
            assistant.role = LLMMessage::Role::Assistant;
            assistant.content = completion.text;
            m_store.appendMessage(req.session_id, assistant);
            final.text = completion.text;
            return final;
        }

        LLMMessage assistant_tools;
        assistant_tools.role = LLMMessage::Role::Assistant;
        assistant_tools.content = completion.text;
        m_store.appendMessage(req.session_id, assistant_tools);

        for(const LLMToolCall& call : completion.tool_calls)
        {
            ToolInvokeRequest invoke;
            invoke.trace_id = req.trace_id;
            invoke.tool_name = call.name;
            invoke.arguments = call.arguments;
            invoke.session = req.session;

            ToolGatewayResult tr = m_gateway.invoke(invoke);
            if(tr.pending_confirmation)
            {
                PendingConfirmation pending;
                pending.confirmation_id = tr.confirmation_id;
                pending.request = invoke;
                pending.request.confirmed = true;
                m_store.setPending(req.session_id, pending);
                final.pending_confirmation = true;
                final.text = "Confirmation required for: " + call.name;
                return final;
            }

            LLMMessage tool_msg;
            tool_msg.role = LLMMessage::Role::Tool;
            tool_msg.tool_call_id = call.id;
            tool_msg.tool_name = call.name;
            tool_msg.content = tr.result.dump();
            m_store.appendMessage(req.session_id, tool_msg);
        }

        opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
    }

    final.text = "Stopped: maximum tool rounds reached.";
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
