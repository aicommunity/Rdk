#ifndef RDK_ULLM_TOOL_TRACE_H
#define RDK_ULLM_TOOL_TRACE_H

#include "../LlmTypes.h"

#include <string>
#include <vector>

namespace RDK::LLM {

struct ConversationState;

nlohmann::json sanitizeToolArgumentsForDisplay(const nlohmann::json& args,
                                            const nlohmann::json& input_schema = {});

void recordTurnToolInvocation(ConversationState& state, const std::string& tool_name,
                              const nlohmann::json& arguments, const ToolGatewayResult& result,
                              int duration_ms = 0,
                              const nlohmann::json& input_schema = {});

std::string formatTurnToolTraceHtml(const std::vector<TurnToolInvocationView>& trace);

} // namespace RDK::LLM

#endif
