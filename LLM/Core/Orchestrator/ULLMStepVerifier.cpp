#include "ULLMStepVerifier.h"

#include "../Domain/ULLMLinkIdentity.h"

#include <sstream>

namespace RDK::LLM {

namespace {

bool verifyLinkExistsInSnapshot(const SuccessCriteria& criteria, URdkDomainAccess& domain,
                                int channel_index, VerifyResult& out)
{
    const std::string from_ln = criteria.params.value("from_long_name", "");
    const std::string to_ln = criteria.params.value("to_long_name", "");
    const std::string from_prop = criteria.params.value("from_property", "");
    const std::string to_prop = criteria.params.value("to_property", "");
    if(from_ln.empty() || to_ln.empty() || from_prop.empty() || to_prop.empty())
    {
        out.detail = "link_exists_missing_quad_fields";
        return false;
    }

    LinkQuad quad;
    quad.from_long_name = from_ln;
    quad.from_property = from_prop;
    quad.to_long_name = to_ln;
    quad.to_property = to_prop;

    nlohmann::json snap;
    const DomainStatus st = domain.listNetSnapshot(snap, channel_index);
    if(st.ok() && snapshotContainsLink(snap, quad))
    {
        out.satisfied = true;
        out.detail = "link_found_in_snapshot";
        return true;
    }
    out.detail = "link_missing_in_snapshot";
    return false;
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

    if(criteria.type == "multi_component_count")
    {
        nlohmann::json snap;
        const DomainStatus st = domain.listNetSnapshot(snap, channel_index);
        if(!st.ok())
        {
            out.detail = st.message;
            return out;
        }
        const auto specs = criteria.params.value("specs", nlohmann::json::array());
        std::ostringstream detail;
        bool all_ok = true;
        for(const nlohmann::json& spec : specs)
        {
            if(!spec.is_object())
                continue;
            const std::string class_name = spec.value("class_name", "");
            const int min_count = spec.value("min_count", 1);
            int count = 0;
            for(const auto& c : snap.value("components", nlohmann::json::array()))
            {
                if(c.value("class_name", "") == class_name)
                    ++count;
            }
            if(count < min_count)
                all_ok = false;
            if(!detail.str().empty())
                detail << "; ";
            detail << class_name << "=" << count << "/" << min_count;
        }
        out.satisfied = all_ok && !specs.empty();
        out.detail = detail.str().empty() ? "no_specs" : detail.str();
        return out;
    }

    if(criteria.type == "link_exists")
    {
        verifyLinkExistsInSnapshot(criteria, domain, channel_index, out);
        return out;
    }

    if(criteria.type == "multi_link_exists")
    {
        const nlohmann::json links = criteria.params.value("links", nlohmann::json::array());
        if(!links.is_array() || links.empty())
        {
            out.detail = "multi_link_exists_empty";
            return out;
        }
        std::ostringstream detail;
        bool all_ok = true;
        for(const auto& link_j : links)
        {
            SuccessCriteria single;
            single.type = "link_exists";
            single.params = link_j.is_object() ? link_j : nlohmann::json::object();
            VerifyResult one;
            if(!verifyLinkExistsInSnapshot(single, domain, channel_index, one))
                all_ok = false;
            if(!detail.str().empty())
                detail << "; ";
            detail << one.detail;
        }
        out.satisfied = all_ok;
        out.detail = detail.str();
        return out;
    }

    out.detail = "unsupported_success_type:" + criteria.type;
    return out;
}

} // namespace RDK::LLM
