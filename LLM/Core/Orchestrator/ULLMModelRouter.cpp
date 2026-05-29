#include "ULLMModelRouter.h"

namespace RDK::LLM {

ModelRoute routeModelForPhase(const ModelTier tier, const LLMProviderProfile& active)
{
    ModelRoute route;
    route.tier = tier;
    route.profile_hint = active.profile_id;
    if(tier == ModelTier::Router)
        route.profile_hint += ":router";
    else if(tier == ModelTier::Utility)
        route.profile_hint += ":utility";
    return route;
}

} // namespace RDK::LLM
