#ifndef RDK_ULLM_AGENT_MANIFEST_BUILDER_H
#define RDK_ULLM_AGENT_MANIFEST_BUILDER_H

#include <string>

#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

std::string buildAgentManifest(const ULLMToolRegistry& registry, const ToolFilter& filter,
                               std::size_t max_chars = 6000,
                               const std::string& user_text = {},
                               const std::string& system_log_summary = {},
                               const std::string& response_language = {});

} // namespace RDK::LLM

#endif
