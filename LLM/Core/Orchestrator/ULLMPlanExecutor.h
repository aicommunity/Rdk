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
};

class ULLMPlanExecutor {
public:
    ULLMPlanExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway);

    PlanExecutionResult execute(ULLMExecutionPlan& plan, const LLMSessionContext& session,
                                const std::string& trace_id);

private:
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
};

} // namespace RDK::LLM

#endif
