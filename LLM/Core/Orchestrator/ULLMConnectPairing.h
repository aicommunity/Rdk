#ifndef RDK_ULLM_CONNECT_PAIRING_H
#define RDK_ULLM_CONNECT_PAIRING_H

#include "ULLMConnectPlanParsing.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

class URdkDomainAccess;
class ULinkPatternCatalog;
class ULLMConnectSemanticsCatalog;

struct PairingCandidate {
    std::string from_long_name;
    std::string to_long_name;
    std::string from_property;
    std::string to_property;
    double score = 0.0;
};

struct PairingRequest {
    std::vector<std::string> endpoints;
    const nlohmann::json* snapshot_components = nullptr;
    URdkDomainAccess* domain = nullptr;
    ULinkPatternCatalog* catalog = nullptr;
    const ULLMConnectSemanticsCatalog* semantics = nullptr;
    int channel_index = 0;
    ConnectTopology topology = ConnectTopology::Sequential;
    std::optional<std::string> hub_long_name;
    int link_count = 0;
    std::string goal_en;
    bool prefer_internal_semantics = false;
};

struct PairingResult {
    std::vector<PairingCandidate> pairs;
    std::vector<std::string> issues;
};

PairingResult buildPairingCandidates(const PairingRequest& req);

} // namespace RDK::LLM

#endif
