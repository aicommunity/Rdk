#ifndef RDK_ULLM_PLAN_REPEAT_POLICY_H
#define RDK_ULLM_PLAN_REPEAT_POLICY_H

#include <string>

namespace RDK::LLM {

/// Tools that honor ExecutionPlanStep.repeat_count in plan/task executors.
bool toolSupportsPlanStepRepeat(const std::string& tool_name);

} // namespace RDK::LLM

#endif
