#include "ULLMSubagentRunner.h"

#include "../LlmModuleInit.h"
#include "ULLMToolFilterBuilder.h"

namespace RDK::LLM {

ULLMSubagentRunner::ULLMSubagentRunner(ILLMProvider& provider, ULLMToolRegistry& registry,
                                       ULLMToolGateway& gateway)
    : m_provider(provider)
    , m_registry(registry)
    , m_gateway(gateway)
{
}

SubagentRunResult ULLMSubagentRunner::runExplore(const SubagentRunRequest& req,
                                                 const std::string& trace_id,
                                                 const std::string& session_id,
                                                 const LLMSessionContext& session)
{
    SubagentRunResult out;
    if(req.task.empty())
    {
        out.ok = false;
        out.summary = "Empty explore task.";
        return out;
    }

    const std::string profile = req.profile.empty() ? "explore" : req.profile;
    if(profile != "explore")
    {
        out.ok = false;
        out.summary = "Unsupported subagent profile: " + profile;
        return out;
    }

    ToolFilter filter =
        buildToolFilter(LLMIntentKind::Query, false, ConfigurationLifecycleAction::None);
    filter.include_write = false;

    std::vector<LLMMessage> messages;
    LLMMessage system;
    system.role = LLMMessage::Role::System;
    system.content =
        "You are a read-only explore subagent. Use only read tools. Summarize findings briefly.";
    messages.push_back(system);
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = req.task;
    messages.push_back(user);

    const int max_rounds = req.max_rounds > 0 ? req.max_rounds : 4;
    for(int round = 0; round < max_rounds; ++round)
    {
        ++out.rounds_used;
        LLMCompletionOptions opts;
        opts.tools_for_api = m_registry.buildOpenAiToolsJson(filter);
        opts.max_tokens = 1024;
        const LLMCompletionResult completion = m_provider.chat(messages, opts);
        if(!completion.ok)
        {
            out.ok = false;
            out.summary = completion.error_message.empty() ? "Subagent provider failed."
                                                           : completion.error_message;
            break;
        }

        if(completion.tool_calls.empty())
        {
            out.summary = completion.text.empty() ? "Explore completed with no text." : completion.text;
            out.ok = true;
            break;
        }

        LLMMessage assistant;
        assistant.role = LLMMessage::Role::Assistant;
        assistant.content = completion.text;
        assistant.assistant_tool_calls = completion.tool_calls;
        messages.push_back(assistant);

        for(const LLMToolCall& call : completion.tool_calls)
        {
            if(call.name == "spawn_explore_subagent")
            {
                ToolGatewayResult denied;
                denied.ok = false;
                denied.message = "Nested subagents are not allowed.";
                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call.id;
                tool_msg.tool_name = call.name;
                tool_msg.content = denied.message;
                messages.push_back(tool_msg);
                continue;
            }
            const LLMToolDefinition* def = m_registry.find(call.name);
            if(def && def->kind == LLMToolKind::Write)
            {
                LLMMessage tool_msg;
                tool_msg.role = LLMMessage::Role::Tool;
                tool_msg.tool_call_id = call.id;
                tool_msg.tool_name = call.name;
                tool_msg.content = "Write tools are denied in explore subagent.";
                messages.push_back(tool_msg);
                continue;
            }

            ToolInvokeRequest invoke;
            invoke.trace_id = trace_id;
            invoke.tool_name = call.name;
            invoke.arguments = call.arguments;
            invoke.session = session;
            invoke.idempotency_key = trace_id + ":sub:" + call.id;
            const ToolGatewayResult tr = m_gateway.invoke(invoke);

            LLMMessage tool_msg;
            tool_msg.role = LLMMessage::Role::Tool;
            tool_msg.tool_call_id = call.id;
            tool_msg.tool_name = call.name;
            tool_msg.content = tr.ok ? tr.message : tr.message;
            if(tr.ok && !tr.result.is_null())
                tool_msg.content += "\n" + tr.result.dump();
            messages.push_back(tool_msg);
        }
    }

    if(out.summary.empty())
        out.summary = "Explore subagent reached round limit.";

    GetAuditLog().append("subagent_explore_completed",
                         {{"task_len", static_cast<int>(req.task.size())},
                          {"profile", profile},
                          {"max_rounds", max_rounds},
                          {"rounds_used", out.rounds_used},
                          {"ok", out.ok}},
                         trace_id, session_id);
    return out;
}

} // namespace RDK::LLM
