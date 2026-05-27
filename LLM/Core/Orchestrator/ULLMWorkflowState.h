#ifndef RDK_ULLM_WORKFLOW_STATE_H
#define RDK_ULLM_WORKFLOW_STATE_H

#include <string>

#include "../LlmTypes.h"

namespace RDK::LLM {

enum class LLMWorkflowPhase {
    Idle,
    Running,
    AwaitingConfirmation,
    Executing,
    TaskExecuting,
    Completed,
    Failed
};

inline const char* workflowPhaseName(LLMWorkflowPhase phase)
{
    switch(phase)
    {
    case LLMWorkflowPhase::Idle:
        return "Idle";
    case LLMWorkflowPhase::Running:
        return "Running";
    case LLMWorkflowPhase::AwaitingConfirmation:
        return "AwaitingConfirmation";
    case LLMWorkflowPhase::Executing:
        return "Executing";
    case LLMWorkflowPhase::TaskExecuting:
        return "TaskExecuting";
    case LLMWorkflowPhase::Completed:
        return "Completed";
    case LLMWorkflowPhase::Failed:
        return "Failed";
    }
    return "Unknown";
}

inline bool workflowTransitionAllowed(LLMWorkflowPhase from, LLMWorkflowPhase to)
{
    if(from == to)
        return true;
    switch(from)
    {
    case LLMWorkflowPhase::Idle:
        return to == LLMWorkflowPhase::Running;
    case LLMWorkflowPhase::Running:
        return to == LLMWorkflowPhase::Executing || to == LLMWorkflowPhase::TaskExecuting
               || to == LLMWorkflowPhase::AwaitingConfirmation || to == LLMWorkflowPhase::Completed
               || to == LLMWorkflowPhase::Failed;
    case LLMWorkflowPhase::Executing:
    case LLMWorkflowPhase::TaskExecuting:
        return to == LLMWorkflowPhase::AwaitingConfirmation || to == LLMWorkflowPhase::Running
               || to == LLMWorkflowPhase::Completed || to == LLMWorkflowPhase::Failed;
    case LLMWorkflowPhase::AwaitingConfirmation:
        return to == LLMWorkflowPhase::Executing || to == LLMWorkflowPhase::TaskExecuting
               || to == LLMWorkflowPhase::Idle || to == LLMWorkflowPhase::Failed;
    case LLMWorkflowPhase::Completed:
    case LLMWorkflowPhase::Failed:
        return to == LLMWorkflowPhase::Idle || to == LLMWorkflowPhase::Running;
    }
    return false;
}

} // namespace RDK::LLM

#endif
