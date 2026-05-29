#include "ULLMToolExposurePolicy.h"

namespace RDK::LLM {

namespace {

void ensureAllowedSet(ToolFilter& filter)
{
    if(!filter.allowed_tool_names)
        filter.allowed_tool_names = std::unordered_set<std::string>{};
}

void addTool(ToolFilter& filter, const char* name)
{
    ensureAllowedSet(filter);
    filter.allowed_tool_names->insert(name);
}

} // namespace

ToolFilter buildToolExposureFilter(const LLMIntentKind intent, const bool write_enabled,
                                   const ConfigurationLifecycleAction lifecycle_action,
                                   const float understanding_confidence)
{
    ToolFilter filter = buildToolFilter(intent, write_enabled, lifecycle_action);
    if(understanding_confidence < 0.45f && intent == LLMIntentKind::Mutate)
    {
        filter.include_write = false;
        addTool(filter, "ask_user");
        addTool(filter, "get_net_snapshot");
        addTool(filter, "search_project_docs");
    }
    addTool(filter, "ask_user");
    addTool(filter, "propose_plan");
    addTool(filter, "search_tools");
    if(intent == LLMIntentKind::Plan)
        addTool(filter, "propose_plan");
    return filter;
}

} // namespace RDK::LLM
