#ifndef RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H
#define RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H

#include <string>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class URdkDomainAccess;

struct WriteArgumentNormalizeResult {
    bool ok = false;
    bool needs_clarification = false;
    std::string error_code;
    std::string message;
    nlohmann::json normalized_arguments;
    nlohmann::json clarification = nlohmann::json::object();
};

/// Whether the tool should resolve component long_name fields before invoke.
bool writeToolNeedsEntityResolution(const std::string& tool_name);

/// Graph mutation writes that require a prior `get_net_snapshot` step in plans.
bool isNetGraphWriteTool(const std::string& tool_name);

WriteArgumentNormalizeResult normalizeWriteToolArguments(const std::string& tool_name,
                                                         nlohmann::json arguments,
                                                         URdkDomainAccess& domain,
                                                         int channel_index,
                                                         const std::string& user_text = "");

} // namespace RDK::LLM

#endif
