#include "ULLMConversationStore.h"

#include <fstream>

#include <filesystem>

namespace fs = std::filesystem;

namespace RDK::LLM {

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

} // namespace RDK::LLM
