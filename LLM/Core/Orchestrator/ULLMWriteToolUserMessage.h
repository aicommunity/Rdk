#ifndef RDK_ULLM_WRITE_TOOL_USER_MESSAGE_H
#define RDK_ULLM_WRITE_TOOL_USER_MESSAGE_H

#include "../LlmTypes.h"

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

nlohmann::json toolGatewayResultForProvider(const ToolGatewayResult& tr);

std::string formatWriteToolUserMessage(const std::string& tool_name, const ToolGatewayResult& tr);

} // namespace RDK::LLM

#endif
