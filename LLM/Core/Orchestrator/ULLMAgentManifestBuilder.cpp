#include "ULLMAgentManifestBuilder.h"

#include <sstream>

namespace RDK::LLM {

std::string buildAgentManifest(const ULLMToolRegistry& registry, const ToolFilter& filter,
                               const std::size_t max_chars)
{
    std::ostringstream oss;
    const std::vector<LLMToolDefinition> tools = registry.listForLlmApi(filter);

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
    oss << "- Prefer one tool call per step, then inspect tool result.\n";
    oss << "- If no tool can satisfy a mutate request, respond NO_SUITABLE_TOOL.\n";

    std::string manifest = oss.str();
    if(manifest.size() > max_chars)
        manifest.resize(max_chars);
    return manifest;
}

} // namespace RDK::LLM
