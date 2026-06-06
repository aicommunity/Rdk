#ifndef RDK_ULLM_TASK_EXECUTOR_H
#define RDK_ULLM_TASK_EXECUTOR_H

#include "ULLMExecutionPlan.h"
#include "../LlmTypes.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"

#include <string>
#include <vector>

namespace RDK::LLM {

struct ConversationState;

struct TaskExecuteOptions {
    bool compensate_on_failure = true;
    int max_step_retries = 1;
    int max_replan_attempts = 1;
    /// When set, successful add/connect steps update session graph memory incrementally.
    ConversationState* conversation_state = nullptr;
};

struct TaskExecuteResult {
    bool ok = false;
    std::string summary;
    std::vector<int> completed_step_ids;
    bool goal_satisfied = false;
};

class ULLMTaskExecutor {
public:
    ULLMTaskExecutor(ULLMToolRegistry& registry, ULLMToolGateway& gateway);

    TaskExecuteResult execute(ULLMExecutionPlan& plan,
                              const LLMSessionContext& session,
                              const std::string& trace_id,
                              TaskExecuteOptions options = {});

private:
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
};

} // namespace RDK::LLM

#endif
