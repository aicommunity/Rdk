#ifndef RDK_URDK_ENTITY_RESOLVER_H
#define RDK_URDK_ENTITY_RESOLVER_H

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "URdkDomainAccess.h"

namespace RDK::LLM {

struct EntityCandidate {
    std::string long_name;
    std::string class_name;
    std::string short_name;
    double score = 0.0;
};

enum class EntityResolutionStatus { Resolved, Ambiguous, NotFound };

struct EntityResolutionResult {
    EntityResolutionStatus status = EntityResolutionStatus::NotFound;
    std::string canonical_long_name;
    std::vector<EntityCandidate> candidates;
};

class URdkEntityResolver {
public:
    explicit URdkEntityResolver(URdkDomainAccess& domain);

    EntityResolutionResult resolveComponent(const std::string& query, int channel_index = 0,
                                            const std::optional<std::string>& class_filter =
                                                std::nullopt) const;

    nlohmann::json toToolJson(const EntityResolutionResult& result) const;

private:
    URdkDomainAccess& m_domain;
};

} // namespace RDK::LLM

#endif
