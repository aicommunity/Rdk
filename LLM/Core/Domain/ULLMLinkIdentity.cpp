#include "ULLMLinkIdentity.h"

namespace RDK::LLM {

bool linkQuadEquals(const LinkQuad& a, const LinkQuad& b)
{
    return a.from_long_name == b.from_long_name && a.from_property == b.from_property
           && a.to_long_name == b.to_long_name && a.to_property == b.to_property;
}

std::string linkQuadDedupKey(const LinkQuad& q)
{
    return q.from_long_name + '\x1f' + q.from_property + '\x1f' + q.to_long_name + '\x1f'
           + q.to_property;
}

bool linkJsonMatchesQuad(const nlohmann::json& link, const LinkQuad& q)
{
    if(!link.is_object())
        return false;
    LinkQuad from_json;
    from_json.from_long_name = link.value("from_long_name", "");
    from_json.from_property = link.value("from_property", "");
    from_json.to_long_name = link.value("to_long_name", "");
    from_json.to_property = link.value("to_property", "");
    return linkQuadEquals(from_json, q);
}

bool linksArrayContains(const nlohmann::json& links, const LinkQuad& q)
{
    if(!links.is_array())
        return false;
    for(const auto& link : links)
    {
        if(linkJsonMatchesQuad(link, q))
            return true;
    }
    return false;
}

bool snapshotContainsLink(const nlohmann::json& snapshot, const LinkQuad& q)
{
    return linksArrayContains(snapshot.value("links", nlohmann::json::array()), q);
}

LinkQuad linkQuadFromJson(const nlohmann::json& j)
{
    LinkQuad q;
    q.from_long_name = j.value("from_long_name", "");
    q.from_property = j.value("from_property", "");
    q.to_long_name = j.value("to_long_name", "");
    q.to_property = j.value("to_property", "");
    return q;
}

nlohmann::json linkQuadToJson(const LinkQuad& q)
{
    return {{"from_long_name", q.from_long_name},
            {"from_property", q.from_property},
            {"to_long_name", q.to_long_name},
            {"to_property", q.to_property}};
}

bool endpointInSubtree(const std::string& endpoint, const std::string& anchor)
{
    if(anchor.empty() || endpoint.empty())
        return false;
    if(endpoint == anchor)
        return true;
    if(endpoint.size() <= anchor.size())
        return false;
    return endpoint.compare(0, anchor.size(), anchor) == 0 && endpoint[anchor.size()] == '.';
}

std::string mapSubtreeEndpoint(const std::string& endpoint, const std::string& from_anchor,
                               const std::string& to_anchor)
{
    if(from_anchor.empty() || to_anchor.empty() || !endpointInSubtree(endpoint, from_anchor))
        return endpoint;
    return to_anchor + endpoint.substr(from_anchor.size());
}

bool linkMatchesSubtreeFilters(const LinkQuad& q, const ModelLinkListFilters& filters)
{
    if(!filters.component_long_name.empty())
    {
        const bool hit = endpointInSubtree(q.from_long_name, filters.component_long_name)
                         || endpointInSubtree(q.to_long_name, filters.component_long_name);
        if(!hit)
            return false;
    }
    if(!filters.from_long_name.empty()
       && !endpointInSubtree(q.from_long_name, filters.from_long_name))
        return false;
    if(!filters.to_long_name.empty() && !endpointInSubtree(q.to_long_name, filters.to_long_name))
        return false;
    return true;
}

std::vector<LinkQuad> filterLinkQuadsBySubtree(const std::vector<LinkQuad>& quads,
                                               const ModelLinkListFilters& filters)
{
    if(filters.component_long_name.empty() && filters.from_long_name.empty()
       && filters.to_long_name.empty())
        return quads;

    std::vector<LinkQuad> out;
    out.reserve(quads.size());
    for(const LinkQuad& q : quads)
    {
        if(linkMatchesSubtreeFilters(q, filters))
            out.push_back(q);
    }
    return out;
}

} // namespace RDK::LLM
