#include "ULLMModelRouter.h"

#include "../LlmModuleInit.h"
#include "../Observability/ULLMAuditLog.h"
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

std::optional<std::string> modelOverrideForRoute(const ModelRoute& route)
{
    if(const LLMProviderProfile* profile = ULLMProviderCatalog::findById(route.profile_hint))
        return profile->model;
    return std::nullopt;
}

void applyModelRouteAudit(const ModelRoute& route, const std::string& trace_id,
                          const std::string& session_id)
{
    const char* tier_name = "Cortex";
    switch(route.tier)
    {
    case ModelTier::Router:
        tier_name = "Router";
        break;
    case ModelTier::Utility:
        tier_name = "Utility";
        break;
    case ModelTier::Cortex:
        break;
    }
    GetAuditLog().append("model_route_selected",
                         {{"tier", tier_name}, {"profile_hint", route.profile_hint}}, trace_id,
                         session_id);
}

} // namespace RDK::LLM
