#ifndef RDK_ULLM_LINK_IDENTITY_H
#define RDK_ULLM_LINK_IDENTITY_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace RDK::LLM {

struct LinkQuad {
    std::string from_long_name;
    std::string from_property;
    std::string to_long_name;
    std::string to_property;
};

bool linkQuadEquals(const LinkQuad& a, const LinkQuad& b);
std::string linkQuadDedupKey(const LinkQuad& q);
bool linkJsonMatchesQuad(const nlohmann::json& link, const LinkQuad& q);
bool linksArrayContains(const nlohmann::json& links, const LinkQuad& q);
bool snapshotContainsLink(const nlohmann::json& snapshot, const LinkQuad& q);

LinkQuad linkQuadFromJson(const nlohmann::json& j);
nlohmann::json linkQuadToJson(const LinkQuad& q);

/// True when endpoint is the anchor or a descendant (`anchor.child…`).
bool endpointInSubtree(const std::string& endpoint, const std::string& anchor);

/// Rewrite endpoint under from_anchor to the isomorphic path under to_anchor.
std::string mapSubtreeEndpoint(const std::string& endpoint, const std::string& from_anchor,
                               const std::string& to_anchor);

struct ModelLinkListFilters {
    std::string component_long_name; ///< incident: from or to in subtree
    std::string from_long_name;      ///< from endpoint in subtree
    std::string to_long_name;        ///< to endpoint in subtree
};

bool linkMatchesSubtreeFilters(const LinkQuad& q, const ModelLinkListFilters& filters);
std::vector<LinkQuad> filterLinkQuadsBySubtree(const std::vector<LinkQuad>& quads,
                                               const ModelLinkListFilters& filters);

} // namespace RDK::LLM

#endif
