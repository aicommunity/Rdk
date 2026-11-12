#ifndef RDK_APPLICATION_TOOL_HELPERS_H
#define RDK_APPLICATION_TOOL_HELPERS_H

#include "../Domain/URdkApplicationCommands.h"
#include "../Gui/ILLMPresentationSink.h"
#include "../LlmTypes.h"
#include <functional>
#include <nlohmann/json.hpp>

namespace RDK::LLM {

void applyPresentationFromCommand(ILLMPresentationSink* sink, const ApplicationCommandResult& cmd);

ToolGatewayResult invokeApplicationTool(
    ILLMPresentationSink* sink,
    std::function<ApplicationCommandResult()> run);

ToolGatewayResult commandResultToGateway(const ApplicationCommandResult& cmd);

} // namespace RDK::LLM

#endif
