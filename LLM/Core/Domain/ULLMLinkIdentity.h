#ifndef RDK_ULLM_LINK_IDENTITY_H
#define RDK_ULLM_LINK_IDENTITY_H

#include <nlohmann/json.hpp>
#include <string>

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

} // namespace RDK::LLM

#endif
