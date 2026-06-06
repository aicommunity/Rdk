#ifndef RDK_ULLM_PLAN_CONFIDENCE_H
#define RDK_ULLM_PLAN_CONFIDENCE_H

#include "../LlmTypes.h"
#include "ULLMExecutionPlan.h"

#include <string>

namespace RDK::LLM {

struct PlanConfirmDecision {
    bool needs_user_confirmation = true;
    std::string reason;
};

PlanConfirmDecision decidePlanConfirmation(const ULLMExecutionPlan& plan,
                                           LLMAutonomousMode mode,
                                           bool auto_apply_writes,
                                           bool has_issues = false);

} // namespace RDK::LLM

#endif
