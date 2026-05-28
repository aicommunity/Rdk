#include "ULLMContextCompactor.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

#include <filesystem>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

bool envEnabled(const char* name)
{
    const char* v = std::getenv(name);
    if(!v)
        return false;
    return v[0] == '1' || std::strcmp(v, "true") == 0 || std::strcmp(v, "TRUE") == 0;
}

int envInt(const char* name, const int default_value)
{
    const char* v = std::getenv(name);
    if(!v || !*v)
        return default_value;
    return std::atoi(v);
}

std::size_t messageChars(const std::vector<LLMMessage>& messages)
{
    std::size_t n = 0;
    for(const LLMMessage& m : messages)
        n += m.content.size();
    return n;
}

void appendFullJsonl(const fs::path& path, const std::vector<LLMMessage>& messages)
{
    std::ofstream out(path, std::ios::app);
    if(!out)
        return;
    for(const LLMMessage& m : messages)
    {
        nlohmann::json line;
        switch(m.role)
        {
        case LLMMessage::Role::System:
            line["role"] = "system";
            break;
        case LLMMessage::Role::Assistant:
            line["role"] = "assistant";
            break;
        case LLMMessage::Role::Tool:
            line["role"] = "tool";
            break;
        case LLMMessage::Role::User:
        default:
            line["role"] = "user";
            break;
        }
        line["content"] = m.content;
        out << line.dump() << '\n';
    }
}

std::string ruleBasedSummary(const std::vector<LLMMessage>& messages)
{
    std::ostringstream oss;
    oss << "Compressed session transcript (rule-based).\n";
    int user_turns = 0;
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::User)
        {
            ++user_turns;
            if(user_turns <= 5)
                oss << "- User: " << m.content.substr(0, 200) << "\n";
        }
        else if(m.role == LLMMessage::Role::Tool && m.tool_name)
        {
            oss << "- Tool " << *m.tool_name << " (" << m.content.size() << " bytes)\n";
        }
    }
    return oss.str();
}

int countUserTurns(const std::vector<LLMMessage>& messages)
{
    int n = 0;
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::User)
            ++n;
    }
    return n;
}

void maskStaleToolMessages(std::vector<LLMMessage>& messages, const int keep_tail_user_turns)
{
    if(!envEnabled("NMSDK_LLM_MASK_STALE_TOOLS"))
        return;
    const int age_rounds = envInt("NMSDK_LLM_MASK_TOOLS_AGE_ROUNDS", 3);
    const int total_users = countUserTurns(messages);
    int users_seen = 0;
    for(LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::User)
            ++users_seen;
        if(m.role != LLMMessage::Role::Tool)
            continue;
        const int users_before = users_seen;
        const int users_after = total_users - users_before;
        if(users_after > keep_tail_user_turns + age_rounds)
        {
            const std::string tool_name = m.tool_name.value_or("unknown");
            m.content = R"({"masked":true,"tool":")" + tool_name + R"(","byte_size":)"
                        + std::to_string(m.content.size()) + "}";
        }
    }
}

} // namespace

bool ULLMContextCompactor::maybeCompact(ConversationState& state, const std::string& session_id,
                                        const std::string& storage_dir)
{
    maskStaleToolMessages(state.messages, envInt("NMSDK_LLM_CONTEXT_COMPACT_KEEP_TAIL", 6));

    if(!envEnabled("NMSDK_LLM_CONTEXT_COMPACT"))
        return false;
    if(state.pending || state.pending_plan || state.pending_tool_arguments)
        return false;

    const std::size_t threshold =
        static_cast<std::size_t>(envInt("NMSDK_LLM_CONTEXT_COMPACT_THRESHOLD", 28000));
    if(messageChars(state.messages) < threshold)
        return false;

    const int keep_tail = envInt("NMSDK_LLM_CONTEXT_COMPACT_KEEP_TAIL", 6);
    std::vector<LLMMessage> tail;
    int users_kept = 0;
    for(auto it = state.messages.rbegin(); it != state.messages.rend(); ++it)
    {
        tail.push_back(*it);
        if(it->role == LLMMessage::Role::User)
        {
            ++users_kept;
            if(users_kept >= keep_tail)
                break;
        }
    }
    std::reverse(tail.begin(), tail.end());

    const std::vector<LLMMessage> head(state.messages.begin(),
                                         state.messages.end() - static_cast<std::ptrdiff_t>(tail.size()));

    if(!storage_dir.empty())
    {
        const fs::path sidecar = fs::path(storage_dir) / (session_id + ".full.jsonl");
        appendFullJsonl(sidecar, head);
    }

    const std::string summary = ruleBasedSummary(head);
    state.session_summary = summary;

    LLMMessage summary_msg;
    summary_msg.role = LLMMessage::Role::System;
    summary_msg.content = "## Session summary\n" + summary;

    state.messages.clear();
    state.messages.push_back(summary_msg);
    state.messages.insert(state.messages.end(), tail.begin(), tail.end());
    return true;
}

} // namespace RDK::LLM
