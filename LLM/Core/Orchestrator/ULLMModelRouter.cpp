#include "ULLMModelRouter.h"

#include "../Settings/ULLMProviderCatalog.h"

namespace RDK::LLM {

ModelRoute routeModelForPhase(const ModelTier tier, const LLMProviderProfile& active)
{
    ModelRoute route;
    route.tier = tier;
    if(tier == ModelTier::Router)
    {
        if(const LLMProviderProfile* lite = ULLMProviderCatalog::findById("ollama-lite"))
            route.profile_hint = lite->profile_id;
        else
            route.profile_hint = active.profile_id + ":router";
    }
    else if(tier == ModelTier::Utility)
        route.profile_hint = active.profile_id + ":utility";
    else
        route.profile_hint = active.profile_id;
    return route;
}

} // namespace RDK::LLM
