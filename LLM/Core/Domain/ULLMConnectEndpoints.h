#ifndef RDK_ULLM_CONNECT_ENDPOINTS_H
#define RDK_ULLM_CONNECT_ENDPOINTS_H

#include "../Orchestrator/ULLMConnectPlanParsing.h"
#include "ULLMModelLinkWalker.h"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace RDK::LLM {

class URdkDomainAccess;
struct ConversationState;

std::vector<std::string> collectRemainingEndpoints(URdkDomainAccess& domain,
                                                   const nlohmann::json& snapshot_components,
                                                   const ParsedConnectGoal& parsed,
                                                   const ConversationState* session,
                                                   int channel_index,
                                                   bool& links_incomplete_out);

} // namespace RDK::LLM

#endif
