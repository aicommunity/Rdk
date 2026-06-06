#ifndef RDK_APPLICATION_TOOL_AUDIT_H
#define RDK_APPLICATION_TOOL_AUDIT_H

#include "../LlmTypes.h"
#include <string>

namespace RDK::LLM {

constexpr const char* kAuditConfigurationPathKey = "_audit_configuration_path";
constexpr const char* kAuditPresentationEffectKey = "_audit_presentation_effect";

std::string presentationEffectToString(LLMPresentationEffect effect);

void attachApplicationToolAuditFields(ToolGatewayResult& gateway,
                                      const ApplicationCommandResult& cmd);

} // namespace RDK::LLM

#endif
