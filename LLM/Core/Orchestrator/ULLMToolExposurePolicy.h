#ifndef RDK_ULLM_TOOL_EXPOSURE_POLICY_H
#define RDK_ULLM_TOOL_EXPOSURE_POLICY_H

#include "ULLMConfigurationLifecycle.h"
#include "ULLMToolFilterBuilder.h"

namespace RDK::LLM {

enum class ToolExposureTier { CoreRead, Explore, Mutate, AgentMeta };

ToolFilter buildToolExposureFilter(LLMIntentKind intent, bool write_enabled,
                                   ConfigurationLifecycleAction lifecycle_action,
                                   float understanding_confidence = 1.0f);

} // namespace RDK::LLM

#endif
