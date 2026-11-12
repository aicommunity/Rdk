#ifndef RDK_ULLM_TOOL_FILTER_BUILDER_H
#define RDK_ULLM_TOOL_FILTER_BUILDER_H

#include "../LlmTypes.h"
#include "ULLMConfigurationLifecycle.h"

namespace RDK::LLM {

ToolFilter buildToolFilter(LLMIntentKind intent, bool write_enabled,
                           ConfigurationLifecycleAction lifecycle_action);

} // namespace RDK::LLM

#endif
