#ifndef RDK_ULLM_MODEL_ROUTER_H
#define RDK_ULLM_MODEL_ROUTER_H

#include "../LlmTypes.h"
#include "../Providers/ILLMProvider.h"

namespace RDK::LLM {

enum class ModelTier { Router, Cortex, Utility };

struct ModelRoute {
    ModelTier tier = ModelTier::Cortex;
    std::string profile_hint;
};

ModelRoute routeModelForPhase(ModelTier tier, const LLMProviderProfile& active);

std::optional<std::string> modelOverrideForRoute(const ModelRoute& route);

void applyModelRouteAudit(const ModelRoute& route, const std::string& trace_id,
                          const std::string& session_id);

} // namespace RDK::LLM

#endif
