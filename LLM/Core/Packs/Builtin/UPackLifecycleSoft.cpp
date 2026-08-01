#include "UPackLifecycleSoft.h"

#include "../../Orchestrator/ULLMConfigurationLifecycle.h"

namespace RDK::LLM {

PackMatch UPackLifecycleSoft::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    const ConfigurationLifecycleAction action =
        detectConfigurationLifecycleAction(snap.req->user_text);
    if(action == ConfigurationLifecycleAction::None)
        return m;
    m.score = 1.0f;
    m.reason = "lifecycle";
    return m;
}

PackHintContribution UPackLifecycleSoft::hints(const PackTurnSnapshot& snap) const
{
    PackHintContribution h;
    if(!snap.req)
        return h;
    const ConfigurationLifecycleAction action =
        detectConfigurationLifecycleAction(snap.req->user_text);
    if(action == ConfigurationLifecycleAction::None)
        return h;
    const bool project_loaded = snap.session && snap.session->project_loaded;
    h.ephemeral_markdown =
        "## Configuration lifecycle\n"
        + configurationLifecycleSystemHint(action, project_loaded);
    h.extra_tool_names = {"create_configuration", "load_configuration", "open_recent_configuration",
                          "save_configuration", "close_configuration", "validate_configuration",
                          "list_recent_configurations", "ask_user"};
    h.act_or_clarify_recovery_tools = h.extra_tool_names;
    return h;
}

} // namespace RDK::LLM
