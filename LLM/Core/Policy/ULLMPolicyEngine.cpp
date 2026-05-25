#include "ULLMPolicyEngine.h"

namespace RDK::LLM {

PolicyDecision ULLMPolicyEngine::checkToolInvoke(const ToolInvokeRequest& req,
                                                  const LLMToolDefinition& tool,
                                                  const URdkDomainAccess& domain) const
{
    (void)req;
    DomainSessionInfo info = domain.sessionInfo();
    const bool project_loaded = info.project_loaded || req.session.project_loaded;
    if(tool.kind == LLMToolKind::Write && !project_loaded)
    {
        return {false, "PROJECT_NOT_LOADED", "Project must be loaded for write tools"};
    }
    if(tool.kind == LLMToolKind::Write && !req.session.llm_write_enabled)
    {
        return {false, "WRITE_DISABLED", "LLM write operations disabled in settings"};
    }
    if(tool.requires_confirmation && !req.confirmed)
    {
        return {true, "", ""};
    }
    return {true, "", ""};
}

} // namespace RDK::LLM
