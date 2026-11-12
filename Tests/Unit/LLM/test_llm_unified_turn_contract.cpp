#include <gtest/gtest.h>

#include "Orchestrator/ULLMTaskPathMode.h"
#include "Orchestrator/ULLMWorkflowState.h"

TEST(UnifiedTurnContract, DefaultTaskPathIsHintOnly)
{
    unsetenv("NMSDK_LLM_TASK_PATH_STRICT");
    EXPECT_EQ(RDK::LLM::resolveTaskPathMode(), RDK::LLM::LLMTaskPathMode::HintOnly);
}

TEST(UnifiedTurnContract, StrictEnvEnablesFastPath)
{
    setenv("NMSDK_LLM_TASK_PATH_STRICT", "1", 1);
    EXPECT_EQ(RDK::LLM::resolveTaskPathMode(), RDK::LLM::LLMTaskPathMode::FastPath);
    unsetenv("NMSDK_LLM_TASK_PATH_STRICT");
}

TEST(UnifiedTurnContract, AwaitingUserInputTransitions)
{
    using RDK::LLM::LLMWorkflowPhase;
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(LLMWorkflowPhase::Running,
                                                     LLMWorkflowPhase::AwaitingUserInput));
    EXPECT_TRUE(RDK::LLM::workflowTransitionAllowed(LLMWorkflowPhase::AwaitingUserInput,
                                                     LLMWorkflowPhase::Running));
}
