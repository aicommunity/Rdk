#include "ApplicationToolHelpers.h"
#include "ApplicationToolAudit.h"

namespace RDK::LLM {

void applyPresentationFromCommand(ILLMPresentationSink* sink, const ApplicationCommandResult& cmd)
{
    if(!sink)
        return;

    LLMPresentationEvent ev;
    ev.effect = cmd.presentation;
    ev.configuration_ini_path = cmd.resolved_configuration_path;
    ev.project_loaded = cmd.update_context && !cmd.project_closed;
    ev.update_context = cmd.update_context;
    ev.project_closed = cmd.project_closed;
    ev.add_to_recent = cmd.add_to_recent;

    if(ev.effect != LLMPresentationEffect::None || ev.update_context || ev.add_to_recent)
        sink->apply(ev);
}

ToolGatewayResult commandResultToGateway(const ApplicationCommandResult& cmd)
{
    ToolGatewayResult r;
    r.ok = cmd.status.ok();
    r.result = cmd.payload;
    if(!r.ok)
    {
        if(cmd.status.code == DomainStatusCode::PolicyDenied)
            r.error_code = "PathNotAllowed";
        else
            r.error_code = "DomainError";
        r.message = cmd.status.message;
    }
    return r;
}

ToolGatewayResult invokeApplicationTool(ILLMPresentationSink* sink,
                                        std::function<ApplicationCommandResult()> run)
{
    ApplicationCommandResult cmd = run();
    ToolGatewayResult r = commandResultToGateway(cmd);
    if(r.ok)
    {
        attachApplicationToolAuditFields(r, cmd);
        applyPresentationFromCommand(sink, cmd);
    }
    return r;
}

} // namespace RDK::LLM
