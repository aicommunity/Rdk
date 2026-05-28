#ifndef RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H
#define RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H

#include "ULLMNameResolution.h"

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

bool writeToolNeedsEntityResolution(const std::string& tool_name);

bool isNetGraphWriteTool(const std::string& tool_name);

struct ConversationState;

WriteArgumentNormalizeResult normalizeWriteToolArguments(const std::string& tool_name,
                                                         nlohmann::json arguments,
                                                         URdkDomainAccess& domain,
                                                         int channel_index,
                                                         const std::string& user_text = "",
                                                         const ConversationState* conversation = nullptr);

} // namespace RDK::LLM

#endif
