#include "ULLMAgentManifestBuilder.h"

#include "../Settings/ULLMResponseLanguage.h"
#include "ULLMLibraryScopeHint.h"
#include "ULLMConnectPlanParsing.h"

#include <sstream>

namespace RDK::LLM {

std::string buildAgentManifest(const ULLMToolRegistry& registry, const ToolFilter& filter,
                               const std::size_t max_chars, const std::string& user_text,
                               const std::string& system_log_summary,
                               const std::string& response_language)
{
    std::ostringstream oss;
    const std::vector<LLMToolDefinition> tools = registry.listForLlmApi(filter);

    if(!response_language.empty())
    {
        oss << "## Response language\n"
            << "- Always respond to the user in "
            << responseLanguageDisplayName(response_language) << " (code: " << response_language
            << "). Tool arguments may stay in English.\n"
            << "- Product identity: NeuroModeler (RDK). Do not invent alternate product names.\n";
    }

    oss << libraryScopeHintManifestSection(detectLibraryScopeFromUserText(user_text));

    if(isConnectGoalText(user_text))
    {
        oss << "## Connect vs Add\n"
            << "- User wants a **link** (connect, link, подключи, связь, соедини): use "
               "connect_components only — never add_component.\n"
            << "- Named names are **subtree anchors**: wiring often uses nested published "
               "ports under those components.\n"
            << "- Inspect first: list_model_links (component/from/to filters) and "
               "get_component_ports (include_nested) before guessing topology.\n"
            << "- connect_components needs from_long_name, from_property (published output), "
               "to_long_name, to_property (published input). Same API as GUI "
               "Model_CreateLinkByName.\n"
            << "- For “same as connected to X”, list links involving X, then replicate ports "
               "onto peer targets with nested long_name prefix rewrite.\n";
    }
    else if(!user_text.empty())
    {
        const std::string lower = user_text;
        if(lower.find("добав") != std::string::npos || lower.find("add ") != std::string::npos)
        {
            oss << "## Add component\n"
                << "- add_component creates a **child** under current kernel CurrentComponent "
                   "when parent_long_name is omitted.\n";
        }
    }

    if(!system_log_summary.empty())
    {
        oss << "## System logging\n";
        oss << "- " << system_log_summary << "\n";
        oss << "- Use read_system_log to inspect logs; tool results may include system_log_excerpt.\n";
    }

    oss << "## Tools (" << tools.size() << " available)\n";
    oss << "- Prefer tools that match the user goal; call search_tools when a needed capability "
           "is missing from this list (it unlocks additional tools for later rounds).\n";
    for(const LLMToolDefinition& tool : tools)
    {
        oss << "- " << tool.name << " [" << (tool.kind == LLMToolKind::Write ? "write" : "read");
        if(tool.requires_confirmation)
            oss << ", hitl";
        oss << "]: " << tool.description << "\n";
    }

    oss << "## Knowledge\n";
    oss << "- Live project graph (components/links on the open configuration): get_net_snapshot, "
           "find_component, get_component_properties — not search_project_docs.\n";
    oss << "- Product docs / how-to / class ClDesc: search_project_docs(scope=docs|sources|all), "
           "describe_class.\n";
    oss << "## Rules\n";
    oss << "- Use write tools only for explicit mutate requests.\n";
    oss << "- Graph mutations: add_component, set_property, connect_components (and disconnect_components) as requested.\n";
    oss << "- connect_components: from_property must be a real output port on the source; to_property a real input on the target (not generic \"output\"/\"input\"). Use get_component_properties when unsure.\n";
    oss << "- Prefer one tool call per step, then inspect tool result.\n";
    oss << "- After a write tool runs, the system emits a deterministic execution summary (not model prose).\n";
    oss << "- If no tool can satisfy a mutate request, respond NO_SUITABLE_TOOL.\n";

    std::string manifest = oss.str();
    if(manifest.size() > max_chars)
        manifest.resize(max_chars);
    return manifest;
}

} // namespace RDK::LLM
