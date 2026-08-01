#include "ULLMCapabilityPackRegistry.h"

#include <algorithm>

#include "../LlmModuleInit.h"
#include "../Observability/ULLMAuditLog.h"

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

RecordedStrategyResult tryRecordedCapabilityPacks(ILLMCapabilityPackRegistry& packs,
                                                  PackTurnSnapshot& snap, float score_threshold)
{
    RecordedStrategyResult out;
    const auto ranked = packs.rank(snap);
    for(const auto& entry : ranked)
    {
        ILLMCapabilityPack* pack = entry.first;
        const PackMatch& match = entry.second;
        if(match.score < score_threshold)
            break;
        GetAuditLog().append("pack_matched",
                             {{"pack_id", pack->id()},
                              {"score", match.score},
                              {"reason", match.reason}},
                             snap.req ? snap.req->trace_id : "",
                             snap.req ? snap.req->session_id : "");
        RecordedStrategyResult tried = pack->tryRecorded(snap);
        if(tried.handled)
        {
            GetAuditLog().append("pack_recorded_fired",
                                 {{"pack_id", pack->id()}, {"score", match.score}},
                                 snap.req ? snap.req->trace_id : "",
                                 snap.req ? snap.req->session_id : "");
            return tried;
        }
    }
    return out;
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

} // namespace RDK::LLM
