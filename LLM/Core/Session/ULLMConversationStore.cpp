#include "ULLMConversationStore.h"

#include "../Orchestrator/ULLMExecutionPlan.h"
#include "ULLMConfirmationExpiry.h"

#include <fstream>

#include <filesystem>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

nlohmann::json sessionContextToJson(const LLMSessionContext& session)
{
    return {{"session_id", session.session_id},
            {"user_name", session.user_name},
            {"user_id", session.user_id},
            {"project_loaded", session.project_loaded},
            {"llm_write_enabled", session.llm_write_enabled},
            {"allow_cloud_llm", session.allow_cloud_llm},
            {"allow_save", session.allow_save},
            {"active_channel_index", session.active_channel_index}};
}

LLMSessionContext sessionContextFromJson(const nlohmann::json& j)
{
    LLMSessionContext session;
    session.session_id = j.value("session_id", "");
    session.user_name = j.value("user_name", "");
    session.user_id = j.value("user_id", 0);
    session.project_loaded = j.value("project_loaded", false);
    session.llm_write_enabled = j.value("llm_write_enabled", true);
    session.allow_cloud_llm = j.value("allow_cloud_llm", false);
    session.allow_save = j.value("allow_save", true);
    session.active_channel_index = j.value("active_channel_index", 0);
    return session;
}

nlohmann::json pendingConfirmationToJson(const PendingConfirmation& pending)
{
    nlohmann::json j;
    j["confirmation_id"] = pending.confirmation_id;
    j["created_at_unix_sec"] = pending.created_at_unix_sec;
    j["trace_id"] = pending.request.trace_id;
    j["tool_name"] = pending.request.tool_name;
    j["arguments"] = pending.request.arguments;
    j["session"] = sessionContextToJson(pending.request.session);
    return j;
}

std::optional<PendingConfirmation> pendingConfirmationFromJson(const nlohmann::json& j)
{
    if(!j.is_object() || !j.contains("confirmation_id"))
        return std::nullopt;
    PendingConfirmation pending;
    pending.confirmation_id = j.value("confirmation_id", "");
    pending.created_at_unix_sec = j.value("created_at_unix_sec", int64_t{0});
    pending.request.trace_id = j.value("trace_id", "");
    pending.request.tool_name = j.value("tool_name", "");
    pending.request.arguments = j.value("arguments", nlohmann::json::object());
    if(j.contains("session"))
        pending.request.session = sessionContextFromJson(j["session"]);
    return pending;
}

} // namespace

void ULLMConversationStore::setStorageDirectory(const std::string& path)
{
    m_storage_dir = path;
    if(!m_storage_dir.empty())
        fs::create_directories(m_storage_dir);
}

nlohmann::json ULLMConversationStore::messageToJson(const LLMMessage& msg)
{
    nlohmann::json j;
    switch(msg.role)
    {
    case LLMMessage::Role::System:
        j["role"] = "system";
        break;
    case LLMMessage::Role::User:
        j["role"] = "user";
        break;
    case LLMMessage::Role::Assistant:
        j["role"] = "assistant";
        break;
    case LLMMessage::Role::Tool:
        j["role"] = "tool";
        break;
    }
    j["content"] = msg.content;
    if(msg.tool_call_id)
        j["tool_call_id"] = *msg.tool_call_id;
    if(msg.tool_name)
        j["tool_name"] = *msg.tool_name;
    if(msg.assistant_tool_calls && !msg.assistant_tool_calls->empty())
    {
        j["tool_calls"] = nlohmann::json::array();
        for(const LLMToolCall& call : *msg.assistant_tool_calls)
        {
            j["tool_calls"].push_back({{"id", call.id},
                                       {"name", call.name},
                                       {"arguments", call.arguments}});
        }
    }
    return j;
}

LLMMessage ULLMConversationStore::messageFromJson(const nlohmann::json& j)
{
    LLMMessage msg;
    const std::string role = j.value("role", "user");
    if(role == "system")
        msg.role = LLMMessage::Role::System;
    else if(role == "assistant")
        msg.role = LLMMessage::Role::Assistant;
    else if(role == "tool")
        msg.role = LLMMessage::Role::Tool;
    else
        msg.role = LLMMessage::Role::User;
    msg.content = j.value("content", "");
    if(j.contains("tool_call_id"))
        msg.tool_call_id = j["tool_call_id"].get<std::string>();
    if(j.contains("tool_name"))
        msg.tool_name = j["tool_name"].get<std::string>();
    if(j.contains("tool_calls") && j["tool_calls"].is_array())
    {
        std::vector<LLMToolCall> calls;
        for(const auto& tc : j["tool_calls"])
        {
            LLMToolCall call;
            call.id = tc.value("id", "");
            call.name = tc.value("name", "");
            call.arguments = tc.value("arguments", nlohmann::json::object());
            calls.push_back(call);
        }
        if(!calls.empty())
            msg.assistant_tool_calls = std::move(calls);
    }
    return msg;
}

bool ULLMConversationStore::loadFromDisk(const std::string& session_id)
{
    if(m_storage_dir.empty())
        return false;
    const fs::path file = fs::path(m_storage_dir) / (session_id + ".json");
    if(!fs::exists(file))
        return false;
    std::ifstream in(file);
    nlohmann::json j;
    in >> j;
    ConversationState state;
    state.session_id = j.value("session_id", session_id);
    const std::string phase = j.value("workflow_phase", "Idle");
    if(phase == "Running")
        state.workflow_phase = LLMWorkflowPhase::Running;
    else if(phase == "AwaitingConfirmation")
        state.workflow_phase = LLMWorkflowPhase::AwaitingConfirmation;
    else if(phase == "Executing")
        state.workflow_phase = LLMWorkflowPhase::Executing;
    else if(phase == "Completed")
        state.workflow_phase = LLMWorkflowPhase::Completed;
    else if(phase == "Failed")
        state.workflow_phase = LLMWorkflowPhase::Failed;
    else
        state.workflow_phase = LLMWorkflowPhase::Idle;
    if(j.contains("messages") && j["messages"].is_array())
    {
        for(const auto& item : j["messages"])
            state.messages.push_back(messageFromJson(item));
    }
    if(j.contains("pending_plan"))
    {
        if(auto plan = executionPlanFromJson(j["pending_plan"]))
            state.pending_plan = std::move(*plan);
    }
    if(j.contains("pending"))
    {
        if(auto pending = pendingConfirmationFromJson(j["pending"]))
            state.pending = std::move(*pending);
    }
    m_sessions[session_id] = std::move(state);
    return true;
}

bool ULLMConversationStore::persistToDisk(const std::string& session_id)
{
    if(m_storage_dir.empty())
        return false;
    auto it = m_sessions.find(session_id);
    if(it == m_sessions.end())
        return false;
    fs::create_directories(m_storage_dir);
    nlohmann::json j;
    j["session_id"] = it->second.session_id;
    j["workflow_phase"] = workflowPhaseName(it->second.workflow_phase);
    j["messages"] = nlohmann::json::array();
    for(const LLMMessage& msg : it->second.messages)
        j["messages"].push_back(messageToJson(msg));
    if(it->second.pending_plan)
        j["pending_plan"] = executionPlanToJson(*it->second.pending_plan);
    if(it->second.pending)
        j["pending"] = pendingConfirmationToJson(*it->second.pending);
    const fs::path file = fs::path(m_storage_dir) / (session_id + ".json");
    std::ofstream out(file);
    if(!out)
        return false;
    out << j.dump(2);
    return true;
}

void ULLMConversationStore::removeFromDisk(const std::string& session_id)
{
    if(m_storage_dir.empty())
        return;
    const fs::path file = fs::path(m_storage_dir) / (session_id + ".json");
    if(fs::exists(file))
        fs::remove(file);
}

void ULLMConversationStore::removeSession(const std::string& session_id)
{
    m_sessions.erase(session_id);
    removeFromDisk(session_id);
}

ConversationState& ULLMConversationStore::getOrCreate(const std::string& session_id)
{
    if(m_sessions.find(session_id) == m_sessions.end())
        loadFromDisk(session_id);
    return m_sessions[session_id];
}

void ULLMConversationStore::appendMessage(const std::string& session_id, const LLMMessage& msg)
{
    m_sessions[session_id].messages.push_back(msg);
    persistToDisk(session_id);
}

void ULLMConversationStore::setPending(const std::string& session_id, PendingConfirmation p)
{
    m_sessions[session_id].pending = std::move(p);
    persistToDisk(session_id);
}

void ULLMConversationStore::clearPending(const std::string& session_id)
{
    m_sessions[session_id].pending.reset();
    persistToDisk(session_id);
}

void ULLMConversationStore::setPendingToolArguments(const std::string& session_id,
                                                      PendingToolArguments p)
{
    m_sessions[session_id].pending_tool_arguments = std::move(p);
}

void ULLMConversationStore::clearPendingToolArguments(const std::string& session_id)
{
    m_sessions[session_id].pending_tool_arguments.reset();
}

bool ULLMConversationStore::expirePendingIfStale(const std::string& session_id, int ttl_seconds)
{
    auto it = m_sessions.find(session_id);
    if(it == m_sessions.end())
        loadFromDisk(session_id);
    ConversationState& state = m_sessions[session_id];
    if(!state.pending)
        return false;
    if(!isPendingConfirmationExpired(*state.pending, ttl_seconds))
        return false;
    state.pending.reset();
    if(state.workflow_phase == LLMWorkflowPhase::AwaitingConfirmation)
        state.workflow_phase = LLMWorkflowPhase::Idle;
    persistToDisk(session_id);
    return true;
}

} // namespace RDK::LLM
