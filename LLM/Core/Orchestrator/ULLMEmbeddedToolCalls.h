#ifndef RDK_ULLM_EMBEDDED_TOOL_CALLS_H
#define RDK_ULLM_EMBEDDED_TOOL_CALLS_H

#include <string>
#include <vector>

#include "../LlmTypes.h"
#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

/// Recover tool calls when the model prints ```json {"name":...} ``` instead of native tool_calls.
std::vector<LLMToolCall> tryExtractEmbeddedToolCalls(const std::string& assistant_text,
                                                     const ULLMToolRegistry& registry);

} // namespace RDK::LLM

#endif
