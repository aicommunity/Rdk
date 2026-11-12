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
    if(st.ok() && snap.value("links_truncated", false))
    {
        bool exists = false;
        if(domain.linkExistsInModel(quad, channel_index, "", exists).ok() && exists)
        {
            out.satisfied = true;
            out.detail = "link_found_in_model";
            return true;
        }
    }
    out.detail = "link_missing_in_snapshot";
    return false;
}

bool verifyComponentUnderParent(const SuccessCriteria& criteria, URdkDomainAccess& domain,
                                int channel_index, VerifyResult& out);

bool verifyComponentExists(const std::string& long_name, URdkDomainAccess& domain, int channel_index,
                           VerifyResult& out)
{
    if(long_name.empty())
    {
        out.detail = "component_exists_missing_long_name";
        return false;
    }
    nlohmann::json found;
    if(domain.findComponentByLongName(long_name, found, channel_index).ok())
    {
        out.satisfied = true;
        out.detail = "component_found:" + long_name;
        return true;
    }
    out.detail = "component_missing:" + long_name;
    return false;
}

bool verifyAddComponentEffect(const SuccessCriteria& criteria, URdkDomainAccess& domain,
                              int channel_index, VerifyResult& out)
{
    const std::string expected_ln = criteria.params.value("long_name", "");
    const std::string parent = criteria.params.value("parent_long_name", "");
    const std::string short_name = criteria.params.value("short_name", "");
    const std::string class_name = criteria.params.value("class_name", "");

    if(!expected_ln.empty())
        return verifyComponentExists(expected_ln, domain, channel_index, out);

    if(!parent.empty() && !short_name.empty())
    {
        std::string resolved;
        if(domain.resolveComponentLongName(short_name, channel_index, resolved, parent).ok()
           && verifyComponentExists(resolved, domain, channel_index, out))
            return true;
    }

    if(!expected_ln.empty() && !parent.empty())
    {
        std::string resolved;
        if(domain.resolveComponentLongName(expected_ln, channel_index, resolved, parent).ok()
           && verifyComponentExists(resolved, domain, channel_index, out))
            return true;
    }

    SuccessCriteria under_parent = criteria;
    under_parent.params.erase("long_name");
    if(!parent.empty() && !class_name.empty())
    {
        if(verifyComponentUnderParent(under_parent, domain, channel_index, out))
            return true;
    }

    if(!expected_ln.empty())
    {
        out.detail = "component_missing:" + expected_ln;
        return false;
    }
    out.detail = "component_under_parent_missing_fields";
    return false;
}

bool verifyComponentUnderParent(const SuccessCriteria& criteria, URdkDomainAccess& domain,
                                int channel_index, VerifyResult& out)
{
    const std::string expected_ln = criteria.params.value("long_name", "");
    if(!expected_ln.empty())
        return verifyAddComponentEffect(criteria, domain, channel_index, out);

    const std::string parent = criteria.params.value("parent_long_name", "");
    const std::string class_name = criteria.params.value("class_name", "");
    if(parent.empty() || class_name.empty())
    {
        out.detail = "component_under_parent_missing_fields";
        return false;
    }

    nlohmann::json snap;
    const DomainStatus st = domain.listNetSnapshot(snap, channel_index);
    if(!st.ok())
    {
        out.detail = st.message;
        return false;
    }

    int count = 0;
    for(const auto& c : snap.value("components", nlohmann::json::array()))
    {
        if(c.value("class_name", "") != class_name)
            continue;
        const std::string ln = c.value("long_name", "");
        if(ln == parent || ln.rfind(parent + "/", 0) == 0)
            ++count;
    }
    out.satisfied = count >= 1;
    out.detail = "component_under_parent count=" + std::to_string(count) + " class=" + class_name;
    return out.satisfied;
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

    if(criteria.type == "component_exists")
    {
        verifyComponentExists(criteria.params.value("long_name", ""), domain, channel_index, out);
        return out;
    }

    if(criteria.type == "component_under_parent")
    {
        verifyAddComponentEffect(criteria, domain, channel_index, out);
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

SuccessCriteria buildPostVerifyCriteria(const std::string& tool_name, const nlohmann::json& args,
                                        const ToolGatewayResult& gateway_result)
{
    SuccessCriteria criteria;
    if(tool_name == "add_component")
    {
        criteria.type = "component_under_parent";
        criteria.params["parent_long_name"] = args.value("parent_long_name", "");
        criteria.params["class_name"] = args.value("class_name", "");
        criteria.params["short_name"] = args.value("short_name", "");
        if(gateway_result.result.contains("long_name")
           && gateway_result.result["long_name"].is_string())
            criteria.params["long_name"] = gateway_result.result["long_name"].get<std::string>();
        if(gateway_result.result.contains("short_name")
           && gateway_result.result["short_name"].is_string())
            criteria.params["short_name"] = gateway_result.result["short_name"].get<std::string>();
        return criteria;
    }

    if(tool_name == "connect_components")
    {
        criteria.type = "link_exists";
        criteria.params["from_long_name"] = args.value("from_long_name", "");
        criteria.params["from_property"] = args.value("from_property", "");
        criteria.params["to_long_name"] = args.value("to_long_name", "");
        criteria.params["to_property"] = args.value("to_property", "");
        return criteria;
    }

    return criteria;
}

VerifyResult runPostToolVerification(const std::string& tool_name, const nlohmann::json& args,
                                     const ToolGatewayResult& gateway_result,
                                     URdkDomainAccess& domain, int channel_index)
{
    VerifyResult out;
    if(!gateway_result.ok)
    {
        out.detail = "gateway_not_ok";
        return out;
    }

    if(tool_name != "add_component" && tool_name != "connect_components")
    {
        out.satisfied = true;
        out.detail = "post_verify_skipped";
        return out;
    }

    if(tool_name == "connect_components")
    {
        const std::string from_ln = args.value("from_long_name", "");
        const std::string to_ln = args.value("to_long_name", "");
        VerifyResult from_check;
        if(!verifyComponentExists(from_ln, domain, channel_index, from_check))
        {
            out.detail = "from_" + from_check.detail;
            return out;
        }
        VerifyResult to_check;
        if(!verifyComponentExists(to_ln, domain, channel_index, to_check))
        {
            out.detail = "to_" + to_check.detail;
            return out;
        }
    }

    const SuccessCriteria criteria = buildPostVerifyCriteria(tool_name, args, gateway_result);
    if(criteria.type.empty())
    {
        out.satisfied = true;
        out.detail = "no_criteria";
        return out;
    }
    return verifySuccessCriteria(criteria, domain, channel_index);
}

} // namespace RDK::LLM
