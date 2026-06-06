#ifndef RDK_ULLM_WRITE_TOOL_POLICY_H
#define RDK_ULLM_WRITE_TOOL_POLICY_H

#include <nlohmann/json.hpp>
#include <string>

namespace RDK::LLM {

/// Low-risk `set_property` targets skip HITL confirmation (metadata / labels only).
bool isLowRiskPropertyName(const std::string& property_name);

/// When true, gateway may invoke the write handler without `req.confirmed`.
bool isConfirmationExemptWriteTool(const std::string& tool_name, const nlohmann::json& arguments);

} // namespace RDK::LLM

#endif
