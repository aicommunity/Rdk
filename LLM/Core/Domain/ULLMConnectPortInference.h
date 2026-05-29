#ifndef RDK_ULLM_CONNECT_PORT_INFERENCE_H
#define RDK_ULLM_CONNECT_PORT_INFERENCE_H

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

class URdkDomainAccess;
class ULinkPatternCatalog;

struct ConnectPortInferenceResult {
    bool ok = false;
    bool needs_clarification = false;
    nlohmann::json clarification = nlohmann::json::object();
    std::string error_code;
    std::string message;
};

ConnectPortInferenceResult inferConnectPorts(nlohmann::json& args, URdkDomainAccess& domain,
                                             ULinkPatternCatalog& catalog, int channel_index);

} // namespace RDK::LLM

#endif
