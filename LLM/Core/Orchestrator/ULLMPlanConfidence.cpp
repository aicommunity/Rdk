#include "ULLMPlanConfidence.h"

namespace RDK::LLM {

PlanConfirmDecision decidePlanConfirmation(const ULLMExecutionPlan& plan,
                                           LLMAutonomousMode mode,
                                           bool auto_apply_writes,
                                           bool has_issues)
{
    PlanConfirmDecision out;
    if(has_issues)
    {
        out.needs_user_confirmation = true;
        out.reason = "plan_has_issues";
        return out;
    }

    const float conf = plan.confidence;
    switch(mode)
    {
    case LLMAutonomousMode::SemiAuto:
        out.needs_user_confirmation = !(auto_apply_writes && conf >= 0.80f);
        out.reason = out.needs_user_confirmation ? "semi_auto_low_confidence" : "semi_auto_auto";
        break;
    case LLMAutonomousMode::Strict:
        out.needs_user_confirmation = !(auto_apply_writes && conf >= 0.95f);
        out.reason = out.needs_user_confirmation ? "strict_low_confidence" : "strict_auto";
        break;
    case LLMAutonomousMode::Off:
    default:
        out.needs_user_confirmation = !(auto_apply_writes && conf >= 0.90f);
        out.reason = out.needs_user_confirmation ? "off_requires_confirm" : "off_auto_high_conf";
        break;
    }
    return out;
}

} // namespace RDK::LLM
