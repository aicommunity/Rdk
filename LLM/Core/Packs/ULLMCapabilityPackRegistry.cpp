#include "ULLMCapabilityPackRegistry.h"

#include <algorithm>
#include <sstream>

#include "../LlmModuleInit.h"
#include "../Observability/ULLMAuditLog.h"
#include "../Session/ULLMWorkingGoals.h"
#include "ULLMCompoundGoal.h"

namespace RDK::LLM {

void ULLMCapabilityPackRegistry::registerPack(std::unique_ptr<ILLMCapabilityPack> pack)
{
    if(pack)
        m_packs.push_back(std::move(pack));
}

std::vector<ILLMCapabilityPack*> ULLMCapabilityPackRegistry::packs() const
{
    std::vector<ILLMCapabilityPack*> out;
    out.reserve(m_packs.size());
    for(const auto& p : m_packs)
        out.push_back(p.get());
    return out;
}

std::vector<std::pair<ILLMCapabilityPack*, PackMatch>>
ULLMCapabilityPackRegistry::rank(const PackTurnSnapshot& snap) const
{
    std::vector<std::pair<ILLMCapabilityPack*, PackMatch>> ranked;
    ranked.reserve(m_packs.size());
    for(const auto& p : m_packs)
    {
        PackMatch m = p->match(snap);
        if(m.score > 0.f)
            ranked.push_back({p.get(), m});
    }
    std::sort(ranked.begin(), ranked.end(),
              [](const auto& a, const auto& b) { return a.second.score > b.second.score; });
    return ranked;
}

namespace {

void seedGoalsForHighPacks(PackTurnSnapshot& snap,
                           const std::vector<std::pair<ILLMCapabilityPack*, PackMatch>>& high)
{
    if(!snap.state || high.empty())
        return;
    if(high.size() == 1)
    {
        const std::string id = workingGoalIdForPack(high.front().first->id());
        upsertWorkingGoal(*snap.state, id, workingGoalTitleForPack(high.front().first->id()));
        return;
    }
    // Multi-goal: replace a single turn_goal seed with per-pack goals.
    snap.state->working_goals.erase(
        std::remove_if(snap.state->working_goals.begin(), snap.state->working_goals.end(),
                       [](const WorkingGoal& g) { return g.id == "turn_goal"; }),
        snap.state->working_goals.end());
    for(const auto& entry : high)
    {
        upsertWorkingGoal(*snap.state, workingGoalIdForPack(entry.first->id()),
                          workingGoalTitleForPack(entry.first->id()));
    }
}

RecordedStrategyResult mergeRecordedResults(PackTurnSnapshot& snap,
                                            const std::vector<RecordedStrategyResult>& parts,
                                            const std::vector<std::string>& fired_ids)
{
    RecordedStrategyResult merged;
    if(parts.empty())
        return merged;
    merged.handled = true;
    std::ostringstream text;
    bool all_ok = true;
    bool awaiting = false;
    for(size_t i = 0; i < parts.size(); ++i)
    {
        const RecordedStrategyResult& p = parts[i];
        if(!p.response.text.empty())
        {
            if(text.tellp() > 0)
                text << "\n";
            text << p.response.text;
        }
        all_ok = all_ok && p.response.ok;
        awaiting = awaiting || p.response.awaiting_user_input || p.response.pending_confirmation
                   || p.response.needs_entity_clarification || p.response.needs_argument_clarification
                   || p.response.needs_tool_disambiguation;
        if(i == 0)
            merged.response = p.response;
    }
    merged.response.text = text.str();
    merged.response.ok = all_ok && !awaiting;
    if(snap.state)
    {
        merged.response.tool_trace = snap.state->current_turn_tool_trace;
        merged.response.working_goals = snap.state->working_goals;
    }
    GetAuditLog().append("pack_multi_recorded",
                         {{"fired_count", static_cast<int>(fired_ids.size())},
                          {"fired", fired_ids},
                          {"ok", merged.response.ok}},
                         snap.req ? snap.req->trace_id : "",
                         snap.req ? snap.req->session_id : "");
    return merged;
}

} // namespace

RecordedStrategyResult tryRecordedCapabilityPacks(ILLMCapabilityPackRegistry& packs,
                                                  PackTurnSnapshot& snap, float score_threshold,
                                                  std::vector<std::string>* matched_ids_out,
                                                  float hint_min_score)
{
    RecordedStrategyResult out;
    const auto ranked = packs.rank(snap);
    if(matched_ids_out)
    {
        matched_ids_out->clear();
        for(const auto& entry : ranked)
        {
            if(entry.second.score < hint_min_score)
                break;
            matched_ids_out->push_back(entry.first->id());
        }
    }

    std::vector<std::pair<ILLMCapabilityPack*, PackMatch>> high;
    for(const auto& entry : ranked)
    {
        if(entry.second.score < score_threshold)
            break;
        high.push_back(entry);
        GetAuditLog().append("pack_matched",
                             {{"pack_id", entry.first->id()},
                              {"score", entry.second.score},
                              {"reason", entry.second.reason}},
                             snap.req ? snap.req->trace_id : "",
                             snap.req ? snap.req->session_id : "");
    }
    if(high.empty())
        return out;

    const std::string user_text = snap.req ? snap.req->user_text : "";
    const bool compound = isCompoundActionableGoal(user_text)
                          || (!snap.planning_text.empty()
                              && isCompoundActionableGoal(snap.planning_text));
    const bool multi = high.size() >= 2 || compound;

    // Dependency order for Recorded attempts (DD-PACK-003).
    std::sort(high.begin(), high.end(), [](const auto& a, const auto& b) {
        return packDependencyRank(a.first->id()) < packDependencyRank(b.first->id());
    });

    seedGoalsForHighPacks(snap, high);

    if(!multi)
    {
        RecordedStrategyResult tried = high.front().first->tryRecorded(snap);
        if(tried.handled)
        {
            GetAuditLog().append("pack_recorded_fired",
                                 {{"pack_id", high.front().first->id()},
                                  {"score", high.front().second.score}},
                                 snap.req ? snap.req->trace_id : "",
                                 snap.req ? snap.req->session_id : "");
        }
        return tried;
    }

    // Multi-goal: run every high pack; do not short-circuit on the first handled.
    std::vector<RecordedStrategyResult> parts;
    std::vector<std::string> fired_ids;
    for(const auto& entry : high)
    {
        RecordedStrategyResult tried = entry.first->tryRecorded(snap);
        if(!tried.handled)
            continue;
        GetAuditLog().append("pack_recorded_fired",
                             {{"pack_id", entry.first->id()},
                              {"score", entry.second.score},
                              {"multi", true}},
                             snap.req ? snap.req->trace_id : "",
                             snap.req ? snap.req->session_id : "");
        parts.push_back(std::move(tried));
        fired_ids.push_back(entry.first->id());
        // Clarification / HITL pauses the chain; remaining packs run after user resumes.
        if(parts.back().response.awaiting_user_input || parts.back().response.pending_confirmation
           || parts.back().response.needs_entity_clarification
           || parts.back().response.needs_argument_clarification
           || parts.back().response.needs_tool_disambiguation)
            break;
    }
    if(parts.empty())
        return out;
    return mergeRecordedResults(snap, parts, fired_ids);
}

std::vector<std::string> collectPackRecoveryTools(ILLMCapabilityPackRegistry& packs,
                                                  const std::vector<std::string>& matched_ids)
{
    std::vector<std::string> out;
    for(ILLMCapabilityPack* pack : packs.packs())
    {
        if(!pack)
            continue;
        const bool matched =
            matched_ids.empty()
            || std::find(matched_ids.begin(), matched_ids.end(), pack->id()) != matched_ids.end();
        if(!matched)
            continue;
        PackTurnSnapshot empty;
        PackHintContribution h = pack->hints(empty);
        for(const std::string& t : h.act_or_clarify_recovery_tools)
        {
            if(std::find(out.begin(), out.end(), t) == out.end())
                out.push_back(t);
        }
    }
    return out;
}

std::string collectPackHintsMarkdown(ILLMCapabilityPackRegistry& packs,
                                     const PackTurnSnapshot& snap, float min_score)
{
    std::string block;
    for(const auto& entry : packs.rank(snap))
    {
        if(entry.second.score < min_score)
            break;
        PackHintContribution h = entry.first->hints(snap);
        if(h.ephemeral_markdown.empty())
            continue;
        if(!block.empty())
            block += "\n\n";
        block += h.ephemeral_markdown;
    }
    return block;
}

std::vector<std::string> collectPackExtraToolNames(ILLMCapabilityPackRegistry& packs,
                                                   const PackTurnSnapshot& snap, float min_score)
{
    std::vector<std::string> out;
    for(const auto& entry : packs.rank(snap))
    {
        if(entry.second.score < min_score)
            break;
        PackHintContribution h = entry.first->hints(snap);
        for(const std::string& t : h.extra_tool_names)
        {
            if(std::find(out.begin(), out.end(), t) == out.end())
                out.push_back(t);
        }
    }
    return out;
}

} // namespace RDK::LLM
