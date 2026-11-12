#include "ULLMPlanRepeatPolicy.h"

namespace RDK::LLM {

bool toolSupportsPlanStepRepeat(const std::string& tool_name)
{
    return tool_name == "add_component";
}

} // namespace RDK::LLM
