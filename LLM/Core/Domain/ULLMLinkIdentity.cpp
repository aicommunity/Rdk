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

} // namespace RDK::LLM
