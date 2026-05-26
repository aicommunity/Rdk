#ifndef RDK_ULLM_DYNAMIC_TOOL_ROUTER_H
#define RDK_ULLM_DYNAMIC_TOOL_ROUTER_H

#include "../LlmTypes.h"

#include <string>

namespace RDK::LLM {

/// TD-036 placeholder: optional embedding/score-based tool subset. Off by default.
class ULLMDynamicToolRouter {
public:
    static bool isEnabled();
    static ToolFilter apply(const ToolFilter& base, const std::string& user_text);
};

} // namespace RDK::LLM

#endif
