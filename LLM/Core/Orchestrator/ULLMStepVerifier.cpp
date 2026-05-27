#include "ULLMStepVerifier.h"

namespace RDK::LLM {

namespace {

bool linkMatches(const nlohmann::json& link, const std::string& from_ln, const std::string& to_ln,
                 const std::string& from_prop, const std::string& to_prop)
{
    const std::string lf = link.value("from_long_name", "");
    const std::string lt = link.value("to_long_name", "");
    if(lf != from_ln || lt != to_ln)
        return false;
    if(!from_prop.empty() && link.value("from_property", "") != from_prop)
        return false;
    if(!to_prop.empty() && link.value("to_property", "") != to_prop)
        return false;
    return true;
}

} // namespace

VerifyResult verifySuccessCriteria(const SuccessCriteria& criteria,
                                   URdkDomainAccess& domain,
                                   int channel_index)
{
    VerifyResult out;
    if(criteria.type == "tool_ok")
    {
        out.satisfied = true;
        out.detail = "tool_ok";
        return out;
    }

    if(criteria.type == "component_count" || criteria.type == "goal_component_count")
    {
        nlohmann::json snap;
        const DomainStatus st = domain.listNetSnapshot(snap, channel_index);
        if(!st.ok())
        {
            out.detail = st.message;
            return out;
        }
        const std::string class_name = criteria.params.value("class_name", "");
        const int min_count = criteria.params.value("min_count", 1);
        int count = 0;
        for(const auto& c : snap.value("components", nlohmann::json::array()))
        {
            if(c.value("class_name", "") == class_name)
                ++count;
        }
        out.satisfied = count >= min_count;
        out.detail = "component_count=" + std::to_string(count) + " min=" + std::to_string(min_count);
        return out;
    }

    if(criteria.type == "link_exists")
    {
        const std::string from_ln = criteria.params.value("from_long_name", "");
        const std::string to_ln = criteria.params.value("to_long_name", "");
        const std::string from_prop = criteria.params.value("from_property", "");
        const std::string to_prop = criteria.params.value("to_property", "");
        nlohmann::json snap;
        const DomainStatus st = domain.listNetSnapshot(snap, channel_index);
        if(st.ok())
        {
            for(const auto& link : snap.value("links", nlohmann::json::array()))
            {
                if(linkMatches(link, from_ln, to_ln, from_prop, to_prop))
                {
                    out.satisfied = true;
                    out.detail = "link_found_in_snapshot";
                    return out;
                }
            }
        }

        nlohmann::json from_comp;
        nlohmann::json to_comp;
        const DomainStatus fs = domain.findComponentByLongName(from_ln, from_comp, channel_index);
        const DomainStatus ts = domain.findComponentByLongName(to_ln, to_comp, channel_index);
        out.satisfied = fs.ok() && ts.ok();
        out.detail = out.satisfied ? "endpoints_exist_no_snapshot_link" : "endpoints_missing";
        return out;
    }

    out.detail = "unsupported_success_type:" + criteria.type;
    return out;
}

} // namespace RDK::LLM
