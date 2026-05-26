#ifndef RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H
#define RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H

#include <string>
#include <utility>
#include <vector>

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

/// Last explicit class token or keyword from user text (for fuzzy class matching).
std::string extractClassNameQuery(const std::string& class_name_field,
                                  const std::string& user_text);

struct RegisteredClassResolution {
    enum class Status { Resolved, Ambiguous, NotFound };
    Status status = Status::NotFound;
    std::string class_name;
    /// class_name + score (1.0 for case-insensitive exact ties)
    std::vector<std::pair<std::string, double>> candidates;
};

/// Match query against registry: exact case → case-insensitive (may be ambiguous) → alias → fuzzy.
RegisteredClassResolution resolveRegisteredClassName(const std::string& query,
                                                     const std::vector<std::string>& registered);

} // namespace RDK::LLM

#endif
