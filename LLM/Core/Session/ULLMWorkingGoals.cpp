#include "ULLMWorkingGoals.h"

#include <sstream>

namespace RDK::LLM {

nlohmann::json workingGoalsToJson(const std::vector<WorkingGoal>& goals)
{
    nlohmann::json arr = nlohmann::json::array();
    for(const WorkingGoal& g : goals)
    {
        nlohmann::json o;
        o["id"] = g.id;
        o["title"] = g.title;
        o["status"] = workingGoalStatusName(g.status);
        o["success_criteria"] = g.success_criteria;
        o["evidence"] = g.evidence;
        arr.push_back(std::move(o));
    }
    return arr;
}

std::vector<WorkingGoal> workingGoalsFromJson(const nlohmann::json& j)
{
    std::vector<WorkingGoal> out;
    if(!j.is_array())
        return out;
    for(const auto& item : j)
    {
        if(!item.is_object())
            continue;
        WorkingGoal g;
        g.id = item.value("id", "");
        g.title = item.value("title", "");
        g.status = workingGoalStatusFromName(item.value("status", "pending"));
        g.success_criteria = item.value("success_criteria", "");
        if(item.contains("evidence") && item["evidence"].is_array())
        {
            for(const auto& e : item["evidence"])
            {
                if(e.is_string())
                    g.evidence.push_back(e.get<std::string>());
            }
        }
        if(!g.id.empty())
            out.push_back(std::move(g));
    }
    return out;
}

std::string formatWorkingGoalsEphemeral(const std::vector<WorkingGoal>& goals, std::size_t max_chars)
{
    if(goals.empty())
        return {};
    std::ostringstream ss;
    ss << "## Working goals\n";
    for(const WorkingGoal& g : goals)
    {
        ss << "- [" << workingGoalStatusName(g.status) << "] " << g.id << ": " << g.title;
        if(!g.success_criteria.empty())
            ss << " (success: " << g.success_criteria << ")";
        if(!g.evidence.empty())
            ss << " evidence=" << g.evidence.size();
        ss << "\n";
    }
    std::string out = ss.str();
    if(out.size() > max_chars)
        out.resize(max_chars);
    return out;
}

void upsertWorkingGoal(ConversationState& state, const std::string& id, const std::string& title,
                       const std::string& success_criteria, WorkingGoalStatus status)
{
    if(id.empty())
        return;
    for(WorkingGoal& g : state.working_goals)
    {
        if(g.id == id)
        {
            if(!title.empty())
                g.title = title;
            if(!success_criteria.empty())
                g.success_criteria = success_criteria;
            g.status = status;
            return;
        }
    }
    WorkingGoal g;
    g.id = id;
    g.title = title.empty() ? id : title;
    g.success_criteria = success_criteria;
    g.status = status;
    state.working_goals.push_back(std::move(g));
}

void appendWorkingGoalEvidence(ConversationState& state, const std::string& goal_id,
                               const std::string& evidence_line)
{
    if(goal_id.empty() || evidence_line.empty())
        return;
    for(WorkingGoal& g : state.working_goals)
    {
        if(g.id != goal_id)
            continue;
        g.evidence.push_back(evidence_line);
        if(g.evidence.size() > 32)
            g.evidence.erase(g.evidence.begin(), g.evidence.begin() + static_cast<long>(g.evidence.size() - 32));
        if(g.status == WorkingGoalStatus::Pending)
            g.status = WorkingGoalStatus::InProgress;
        return;
    }
}

void markWorkingGoalStatus(ConversationState& state, const std::string& goal_id,
                           WorkingGoalStatus status)
{
    for(WorkingGoal& g : state.working_goals)
    {
        if(g.id == goal_id)
        {
            g.status = status;
            return;
        }
    }
}

void ensureTurnWorkingGoal(ConversationState& state, const std::string& user_text)
{
    if(user_text.empty())
        return;
    if(!state.working_goals.empty())
        return;
    std::string title = user_text;
    if(title.size() > 120)
        title = title.substr(0, 117) + "...";
    upsertWorkingGoal(state, "turn_goal", title, {}, WorkingGoalStatus::InProgress);
}

} // namespace RDK::LLM
