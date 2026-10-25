#include <gtest/gtest.h>

#include "Orchestrator/ULLMModelRouter.h"
#include "Settings/ULLMProviderCatalog.h"

using namespace RDK::LLM;

TEST(ModelRouter, LiteProfileForRouterTier)
{
    LLMProviderProfile active;
    active.profile_id = "ollama-local";
    const ModelRoute route = routeModelForPhase(ModelTier::Router, active);
    EXPECT_EQ(route.profile_hint, "ollama-lite");
}

TEST(ModelRouter, CortexPrefersThinkingProfile)
{
    LLMProviderProfile active;
    active.profile_id = "ollama-local";
    const ModelRoute route = routeModelForPhase(ModelTier::Cortex, active);
    EXPECT_EQ(route.profile_hint, "ollama-thinking");
}
