#include "ApplicationToolAudit.h"

namespace RDK::LLM {

std::string presentationEffectToString(LLMPresentationEffect effect)
{
    switch(effect)
    {
    case LLMPresentationEffect::None:
        return "None";
    case LLMPresentationEffect::ContextOnly:
        return "ContextOnly";
    case LLMPresentationEffect::DiagramRefresh:
        return "DiagramRefresh";
    case LLMPresentationEffect::FullShellRefresh:
        return "FullShellRefresh";
    }
    return "None";
}

void attachApplicationToolAuditFields(ToolGatewayResult& gateway,
                                      const ApplicationCommandResult& cmd)
{
    if(!cmd.resolved_configuration_path.empty())
        gateway.result[kAuditConfigurationPathKey] = cmd.resolved_configuration_path;
    gateway.result[kAuditPresentationEffectKey] = presentationEffectToString(cmd.presentation);
}

} // namespace RDK::LLM
