#include "ULLMConversationStore.h"

#include "../Orchestrator/ULLMExecutionPlan.h"
#include "ULLMConfirmationExpiry.h"

#include <fstream>

#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

std::string autonomousModeToString(LLMAutonomousMode mode)
{
    switch(mode)
    {
    case LLMAutonomousMode::Strict:
        return "strict";
    case LLMAutonomousMode::SemiAuto:
        return "semi_auto";
    case LLMAutonomousMode::Off:
    default:
        return "off";
    }
}

LLMAutonomousMode autonomousModeFromString(const std::string& s)
{
    if(s == "strict")
        return LLMAutonomousMode::Strict;
    if(s == "semi_auto")
        return LLMAutonomousMode::SemiAuto;
    return LLMAutonomousMode::Off;
}

nlohmann::json sessionContextToJson(const LLMSessionContext& session)
{
    return {{"session_id", session.session_id},
            {"user_name", session.user_name},
            {"user_id", session.user_id},
            {"project_loaded", session.project_loaded},
            {"llm_write_enabled", session.llm_write_enabled},
            {"auto_apply_writes", session.auto_apply_writes},
            {"autonomous_mode", autonomousModeToString(session.autonomous_mode)},
            {"autonomous_steps_taken", session.autonomous_steps_taken},
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
    session.auto_apply_writes = j.value("auto_apply_writes", false);
    session.autonomous_mode = autonomousModeFromString(j.value("autonomous_mode", "off"));
    session.autonomous_steps_taken = j.value("autonomous_steps_taken", 0);
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

std::string lifecycleActionToString(ConfigurationLifecycleAction action)
{
    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        return "create";
    case ConfigurationLifecycleAction::Load:
        return "load";
    case ConfigurationLifecycleAction::Save:
        return "save";
    case ConfigurationLifecycleAction::None:
    default:
        return "none";
    }
}

ConfigurationLifecycleAction lifecycleActionFromString(const std::string& value)
{
    if(value == "create")
        return ConfigurationLifecycleAction::Create;
    if(value == "load")
        return ConfigurationLifecycleAction::Load;
    if(value == "save")
        return ConfigurationLifecycleAction::Save;
    return ConfigurationLifecycleAction::None;
}

std::string pendingDisambiguationKindToString(PendingDisambiguationKind kind)
{
    switch(kind)
    {
    case PendingDisambiguationKind::Class:
        return "class";
    case PendingDisambiguationKind::Component:
        return "component";
    case PendingDisambiguationKind::None:
    default:
        return "none";
    }
}

PendingDisambiguationKind pendingDisambiguationKindFromString(const std::string& value)
{
    if(value == "class")
        return PendingDisambiguationKind::Class;
    if(value == "component")
        return PendingDisambiguationKind::Component;
    return PendingDisambiguationKind::None;
}

std::string intentKindToString(LLMIntentKind kind)
{
    switch(kind)
    {
    case LLMIntentKind::Query:
        return "query";
    case LLMIntentKind::Mutate:
        return "mutate";
    case LLMIntentKind::Explain:
        return "explain";
    case LLMIntentKind::Plan:
        return "plan";
    case LLMIntentKind::Auto:
    default:
        return "auto";
    }
}

LLMIntentKind intentKindFromString(const std::string& value)
{
    if(value == "query")
        return LLMIntentKind::Query;
    if(value == "mutate")
        return LLMIntentKind::Mutate;
    if(value == "explain")
        return LLMIntentKind::Explain;
    if(value == "plan")
        return LLMIntentKind::Plan;
    return LLMIntentKind::Auto;
}

nlohmann::json toolArgumentFieldSpecToJson(const ToolArgumentFieldSpec& field)
{
    return {{"name", field.name},
            {"type", field.type},
            {"description", field.description},
            {"required", field.required}};
}

ToolArgumentFieldSpec toolArgumentFieldSpecFromJson(const nlohmann::json& j)
{
    ToolArgumentFieldSpec field;
    field.name = j.value("name", "");
    field.type = j.value("type", "string");
    field.description = j.value("description", "");
    field.required = j.value("required", true);
    return field;
}

nlohmann::json pendingToolArgumentsToJson(const PendingToolArguments& pending)
{
    nlohmann::json j;
    j["tool_name"] = pending.tool_name;
    j["action"] = lifecycleActionToString(pending.action);
    j["partial_arguments"] = pending.partial_arguments;
    j["created_at_unix_sec"] = pending.created_at_unix_sec;
    j["disambiguation_kind"] = pendingDisambiguationKindToString(pending.disambiguation_kind);
    j["disambiguation_field"] = pending.disambiguation_field;
    j["disambiguation_candidates"] = pending.disambiguation_candidates;
    j["class_disambiguation_candidates"] = pending.class_disambiguation_candidates;
    j["missing_fields"] = nlohmann::json::array();
    for(const ToolArgumentFieldSpec& field : pending.missing_fields)
        j["missing_fields"].push_back(toolArgumentFieldSpecToJson(field));
    return j;
}

std::optional<PendingToolArguments> pendingToolArgumentsFromJson(const nlohmann::json& j)
{
    if(!j.is_object())
        return std::nullopt;
    PendingToolArguments pending;
    pending.tool_name = j.value("tool_name", "");
    if(pending.tool_name.empty())
        return std::nullopt;
    pending.action = lifecycleActionFromString(j.value("action", "none"));
    pending.partial_arguments = j.value("partial_arguments", nlohmann::json::object());
    pending.created_at_unix_sec = j.value("created_at_unix_sec", int64_t{0});
    pending.disambiguation_kind =
        pendingDisambiguationKindFromString(j.value("disambiguation_kind", "none"));
    pending.disambiguation_field = j.value("disambiguation_field", "");
    pending.disambiguation_candidates =
        j.value("disambiguation_candidates", nlohmann::json::array());
    pending.class_disambiguation_candidates =
        j.value("class_disambiguation_candidates", nlohmann::json::array());
    if(j.contains("missing_fields") && j["missing_fields"].is_array())
    {
        for(const nlohmann::json& item : j["missing_fields"])
        {
            if(item.is_object())
                pending.missing_fields.push_back(toolArgumentFieldSpecFromJson(item));
        }
    }
    return pending;
}

std::string redactSensitiveText(const std::string& text)
{
    std::string out = text;
    const std::vector<std::regex> patterns = {
        std::regex(R"((api[_-]?key\s*[:=]\s*)([^\s"']+))", std::regex::icase),
        std::regex(R"((token\s*[:=]\s*)([^\s"']+))", std::regex::icase),
        std::regex(R"((password\s*[:=]\s*)([^\s"']+))", std::regex::icase),
        std::regex(R"(sk-[A-Za-z0-9]{16,})", std::regex::icase)};
    for(const std::regex& pattern : patterns)
        out = std::regex_replace(out, pattern, "$1[REDACTED]");
    return out;
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
    j["content"] = redactSensitiveText(msg.content);
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
    try
    {
        in >> j;
    }
    catch(const std::exception&)
    {
        return false;
    }
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
    state.last_user_text_original = j.value("last_user_text_original", "");
    state.last_user_text_en = j.value("last_user_text_en", "");
    state.intent_contract_kind = intentKindFromString(j.value("intent_contract_kind", "auto"));
    state.intent_contract_confidence = j.value("intent_contract_confidence", 0.0f);
    state.intent_contract_requires_confirmation_for_writes =
        j.value("intent_contract_requires_confirmation_for_writes", true);
    if(j.contains("pending"))
    {
        if(auto pending = pendingConfirmationFromJson(j["pending"]))
            state.pending = std::move(*pending);
    }
    if(j.contains("pending_tool_arguments"))
    {
        if(auto pending_args = pendingToolArgumentsFromJson(j["pending_tool_arguments"]))
            state.pending_tool_arguments = std::move(*pending_args);
    }
    else if(j.value("clarification_lost", false))
    {
        state.pending_tool_arguments.reset();
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
    if(!it->second.last_user_text_original.empty())
        j["last_user_text_original"] = it->second.last_user_text_original;
    if(!it->second.last_user_text_en.empty())
        j["last_user_text_en"] = it->second.last_user_text_en;
    j["intent_contract_kind"] = intentKindToString(it->second.intent_contract_kind);
    j["intent_contract_confidence"] = it->second.intent_contract_confidence;
    j["intent_contract_requires_confirmation_for_writes"] =
        it->second.intent_contract_requires_confirmation_for_writes;
    if(it->second.pending)
        j["pending"] = pendingConfirmationToJson(*it->second.pending);
    if(it->second.pending_tool_arguments)
        j["pending_tool_arguments"] = pendingToolArgumentsToJson(*it->second.pending_tool_arguments);
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
    persistToDisk(session_id);
}

void ULLMConversationStore::clearPendingToolArguments(const std::string& session_id)
{
    m_sessions[session_id].pending_tool_arguments.reset();
    persistToDisk(session_id);
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
