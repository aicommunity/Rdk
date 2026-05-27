#include "ULLMStepVerifier.h"

namespace RDK::LLM {

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
        // Current snapshot may not always expose links in all environments.
        // Keep a conservative check using endpoint existence.
        const std::string from_ln = criteria.params.value("from_long_name", "");
        const std::string to_ln = criteria.params.value("to_long_name", "");
        nlohmann::json from_comp;
        nlohmann::json to_comp;
        const DomainStatus fs = domain.findComponentByLongName(from_ln, from_comp, channel_index);
        const DomainStatus ts = domain.findComponentByLongName(to_ln, to_comp, channel_index);
        out.satisfied = fs.ok() && ts.ok();
        out.detail = out.satisfied ? "endpoints_exist" : "endpoints_missing";
        return out;
    }

    out.detail = "unsupported_success_type:" + criteria.type;
    return out;
}

} // namespace RDK::LLM
