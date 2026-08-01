#include "UPackConnect.h"

#include "../../Orchestrator/ULLMConnectPlanParsing.h"
#include "../../Orchestrator/ULLMContextKnowledgeBlocks.h"

namespace RDK::LLM {

PackMatch UPackConnect::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!isConnectGoalText(snap.req->user_text) && !isConnectGoalText(snap.planning_text))
        return m;
    m.score = 1.0f;
    m.reason = "connect";
    return m;
}

PackHintContribution UPackConnect::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown = buildConnectInspectHintBlock();
    h.extra_tool_names = {"connect_components", "list_model_links", "get_component_ports",
                          "get_net_snapshot", "find_component", "ask_user"};
    h.act_or_clarify_recovery_tools = {"connect_components", "list_model_links",
                                       "get_component_ports", "ask_user"};
    return h;
}

} // namespace RDK::LLM
