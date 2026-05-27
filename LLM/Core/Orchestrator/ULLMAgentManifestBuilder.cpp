#include "ULLMAgentManifestBuilder.h"

#include "ULLMLibraryScopeHint.h"

#include <sstream>

namespace RDK::LLM {

std::string buildAgentManifest(const ULLMToolRegistry& registry, const ToolFilter& filter,
                               const std::size_t max_chars, const std::string& user_text)
{
    std::ostringstream oss;
    const std::vector<LLMToolDefinition> tools = registry.listForLlmApi(filter);

    oss << libraryScopeHintManifestSection(detectLibraryScopeFromUserText(user_text));

    oss << "## Tools (" << tools.size() << " available)\n";
    for(const LLMToolDefinition& tool : tools)
    {
        oss << "- " << tool.name << " [" << (tool.kind == LLMToolKind::Write ? "write" : "read");
        if(tool.requires_confirmation)
            oss << ", hitl";
        oss << "]: " << tool.description << "\n";
    }

    oss << "## Knowledge\n";
    oss << "- search_project_docs(scope=docs|sources|all) for product docs and implementation.\n";
    oss << "## Rules\n";
    oss << "- Use write tools only for explicit mutate requests.\n";
    oss << "- Graph mutations: add_component, set_property, connect_components (and disconnect_components) as requested.\n";
    oss << "- connect_components: from_property must be a real output port on the source; to_property a real input on the target (not generic \"output\"/\"input\"). Use get_component_properties when unsure.\n";
    oss << "- Prefer one tool call per step, then inspect tool result.\n";
    oss << "- If no tool can satisfy a mutate request, respond NO_SUITABLE_TOOL.\n";

    std::string manifest = oss.str();
    if(manifest.size() > max_chars)
        manifest.resize(max_chars);
    return manifest;
}

} // namespace RDK::LLM
