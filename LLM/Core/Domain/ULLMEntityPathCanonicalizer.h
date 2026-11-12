#ifndef RDK_ULLM_ENTITY_PATH_CANONICALIZER_H
#define RDK_ULLM_ENTITY_PATH_CANONICALIZER_H

#include "../LlmTypes.h"
#include "URdkDomainAccess.h"

#include <string>
#include <vector>

namespace RDK::LLM {

std::vector<std::string> entityPathFieldsFromSchema(const nlohmann::json& input_schema);

/// Resolve relative component hints to canonical long_name under pinned GUI scope.
bool canonicalizeEntityPaths(const std::string& tool_name, nlohmann::json& arguments,
                             URdkDomainAccess& domain, int channel_index,
                             const LLMGuiContextSnapshot& pin,
                             const nlohmann::json& input_schema);

} // namespace RDK::LLM

#endif
