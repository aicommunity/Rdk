#include "URdkEntityResolver.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

static std::string toLower(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

URdkEntityResolver::URdkEntityResolver(URdkDomainAccess& domain)
    : m_domain(domain)
{
}

EntityResolutionResult URdkEntityResolver::resolveComponent(
    const std::string& query, int channel_index,
    const std::optional<std::string>& class_filter) const
{
    EntityResolutionResult result;
    const std::string qlower = toLower(query);

    nlohmann::json snap;
    if(!m_domain.listNetSnapshot(snap, channel_index, 500).ok())
        return result;

    for(const auto& c : snap["components"])
    {
        const std::string sn = c.value("short_name", "");
        const std::string ln = c.value("long_name", "");
        const std::string cn = c.value("class_name", "");
        if(class_filter && cn != *class_filter)
            continue;

        double score = 0.0;
        const std::string sn_lower = toLower(sn);
        const std::string ln_lower = toLower(ln);
        if(sn == query || ln == query)
            score = 1.0;
        else if(sn_lower == qlower || ln_lower == qlower)
            score = 0.95;
        else if(ln_lower.size() >= qlower.size() &&
                ln_lower.compare(ln_lower.size() - qlower.size(), qlower.size(), qlower) == 0)
            score = 0.9;
        else if(sn_lower.find(qlower) != std::string::npos || ln_lower.find(qlower) != std::string::npos)
            score = 0.65;

        if(score > 0.0)
        {
            result.candidates.push_back(
                {ln, cn, sn, score});
        }
    }

    std::sort(result.candidates.begin(), result.candidates.end(),
              [](const EntityCandidate& a, const EntityCandidate& b) { return a.score > b.score; });

    if(result.candidates.empty())
        return result;

    if(result.candidates.size() == 1)
    {
        result.status = EntityResolutionStatus::Resolved;
        result.canonical_long_name = result.candidates[0].long_name;
        return result;
    }

    const double top = result.candidates[0].score;
    const double second = result.candidates[1].score;
    if(top >= 0.9 && (top - second) >= 0.15)
    {
        result.status = EntityResolutionStatus::Resolved;
        result.canonical_long_name = result.candidates[0].long_name;
        return result;
    }

    result.status = EntityResolutionStatus::Ambiguous;
    return result;
}

nlohmann::json URdkEntityResolver::toToolJson(const EntityResolutionResult& result) const
{
    nlohmann::json out;
    out["resolved"] = (result.status == EntityResolutionStatus::Resolved);
    out["ambiguous"] = (result.status == EntityResolutionStatus::Ambiguous);
    out["not_found"] = (result.status == EntityResolutionStatus::NotFound);
    if(result.status == EntityResolutionStatus::Resolved)
        out["canonical_long_name"] = result.canonical_long_name;
    out["candidates"] = nlohmann::json::array();
    for(const EntityCandidate& c : result.candidates)
    {
        out["candidates"].push_back({{"long_name", c.long_name},
                                     {"class_name", c.class_name},
                                     {"short_name", c.short_name},
                                     {"score", c.score}});
    }
    return out;
}

} // namespace RDK::LLM
