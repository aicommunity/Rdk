#ifndef RDK_ULLM_CONNECT_PLAN_LLM_FALLBACK_H
#define RDK_ULLM_CONNECT_PLAN_LLM_FALLBACK_H

#include "ULLMConnectPlanBuilder.h"

namespace RDK::LLM {

class ILLMProvider;

ConnectPlanBuildResult tryBuildConnectPlanViaLlm(const ConnectPlanBuildRequest& req,
                                                 ILLMProvider& provider);

} // namespace RDK::LLM

#endif
