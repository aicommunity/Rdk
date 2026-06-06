#ifndef RDK_ULLM_PLAN_EXECUTOR_H
#define RDK_ULLM_PLAN_EXECUTOR_H

#include "ULLMExecutionPlan.h"
#include "../LlmTypes.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

struct PlanExecutionResult {
    bool ok = true;
    std::string summary;
    std::vector<int> completed_step_ids;
    std::vector<int> failed_step_ids;
    std::string compensation_note;
    int compensation_steps_applied = 0;
    bool paused_for_resume = false;
    int checkpoint_after_step_id = 0;
};

struct PlanExecuteOptions {
    bool resume = false;
    /// When false, failed runs keep a checkpoint instead of auto-compensating (TD-023).
    bool compensate_on_failure = true;
};

inline PlanExecuteOptions planExecuteWithCheckpointOnFailure()
{
    PlanExecuteOptions o;
    o.compensate_on_failure = false;
    return o;
}

inline PlanExecuteOptions planExecuteResume()
{
    PlanExecuteOptions o;
    o.resume = true;
    o.compensate_on_failure = false;
    return o;
}

class ULLMPlanExecutor {
public:
    ULLMPlanExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway);

    PlanExecutionResult execute(ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                const std::string& trace_id,
                                PlanExecuteOptions options = {});

    int compensateCompletedWrites(const ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                  const std::string& trace_id, std::string& note_out);

private:
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
};

} // namespace RDK::LLM

#endif
