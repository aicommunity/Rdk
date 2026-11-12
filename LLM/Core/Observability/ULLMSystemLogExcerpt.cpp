#include "ULLMSystemLogExcerpt.h"

#include "../Tools/ULLMToolGateway.h"
#include "../Orchestrator/ULLMWriteToolUserMessage.h"

namespace RDK::LLM {

nlohmann::json toolJsonWithSystemLogExcerpt(const ToolGatewayResult& tr, const std::string& excerpt)
{
    nlohmann::json j = toolGatewayResultForProvider(tr);
    if(!excerpt.empty())
        j["system_log_excerpt"] = excerpt;
    return j;
}

} // namespace RDK::LLM
