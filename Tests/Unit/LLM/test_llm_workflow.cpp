#include <gtest/gtest.h>

#include "Orchestrator/ULLMWorkflowState.h"

TEST(LlmWorkflow, AllowsIdleToRunning)
{
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(RDK::LLM::LLMWorkflowPhase::Idle,
                                                     RDK::LLM::LLMWorkflowPhase::Running));
}

TEST(LlmWorkflow, BlocksIdleToExecuting)
{
    EXPECT_FALSE(RDK::LLM::workflowTransitionAllowed(RDK::LLM::LLMWorkflowPhase::Idle,
                                                      RDK::LLM::LLMWorkflowPhase::Executing));
}

TEST(LlmWorkflow, AwaitingConfirmationToIdle)
{
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(
        RDK::LLM::LLMWorkflowPhase::AwaitingConfirmation, RDK::LLM::LLMWorkflowPhase::Idle));
}

TEST(LlmWorkflow, RunningToUnderstanding)
{
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(RDK::LLM::LLMWorkflowPhase::Running,
                                                     RDK::LLM::LLMWorkflowPhase::Understanding));
}

TEST(LlmWorkflow, RunningToAwaitingUserInput)
{
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(
        RDK::LLM::LLMWorkflowPhase::Running, RDK::LLM::LLMWorkflowPhase::AwaitingUserInput));
}
