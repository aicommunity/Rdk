#include "URdkEntityResolver.h"

#include "ULLMNameResolution.h"

namespace RDK::LLM {

URdkEntityResolver::URdkEntityResolver(URdkDomainAccess& domain)
    : m_domain(domain)
{
}

EntityResolutionResult URdkEntityResolver::resolveComponent(
    const std::string& query, int channel_index,
    const std::optional<std::string>& class_filter) const
{
    EntityResolutionResult result;

    nlohmann::json snap;
    if(!m_domain.listNetSnapshot(snap, channel_index, 500).ok())
        return result;

    const ComponentEntityResolution resolved =
        resolveComponentEntity(query, snap["components"], class_filter);

    if(resolved.status == ComponentEntityResolution::Status::Resolved)
    {
        result.status = EntityResolutionStatus::Resolved;
        result.canonical_long_name = resolved.canonical_long_name;
        return result;
    }

    if(resolved.status == ComponentEntityResolution::Status::Ambiguous)
    {
        result.status = EntityResolutionStatus::Ambiguous;
        for(const auto& [ln, score] : resolved.candidates)
        {
            std::string cn;
            std::string sn;
            for(const nlohmann::json& c : snap["components"])
            {
                if(c.value("long_name", "") == ln)
                {
                    cn = c.value("class_name", "");
                    sn = c.value("short_name", "");
                    break;
                }
            }
            result.candidates.push_back({ln, cn, sn, score});
        }
        return result;
    }

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
