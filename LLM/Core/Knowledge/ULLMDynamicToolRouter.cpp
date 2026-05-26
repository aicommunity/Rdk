#include "ULLMDynamicToolRouter.h"

#include <cstdlib>

namespace RDK::LLM {

bool ULLMDynamicToolRouter::isEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");
    return v && v[0] == '1';
}

ToolFilter ULLMDynamicToolRouter::apply(const ToolFilter& base, const std::string& /*user_text*/)
{
    if(!isEnabled())
        return base;
    // Post-MVP TD-036: score/embedding subset; until then static filter only.
    return base;
}

} // namespace RDK::LLM
