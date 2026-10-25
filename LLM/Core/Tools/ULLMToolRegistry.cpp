#include "ULLMToolRegistry.h"

namespace RDK::LLM {

void ULLMToolRegistry::registerTool(const LLMToolDefinition& def, ToolHandler handler)
{
    m_tools[def.name] = Entry{def, std::move(handler)};
}

const LLMToolDefinition* ULLMToolRegistry::find(const std::string& name) const
{
    auto it = m_tools.find(name);
    if(it == m_tools.end())
        return nullptr;
    return &it->second.def;
}

std::vector<LLMToolDefinition> ULLMToolRegistry::listForLlmApi(const ToolFilter& filter) const
{
    std::vector<LLMToolDefinition> out;
    for(const auto& kv : m_tools)
    {
        const LLMToolDefinition& d = kv.second.def;
        if(d.name == "disconnect_components")
            continue;
        // Writes gated solely by include_write (packs may force it under Query intent).
        if(!filter.include_write && d.kind == LLMToolKind::Write)
            continue;
        if(filter.allowed_tool_names && filter.allowed_tool_names->count(d.name) == 0)
            continue;
        out.push_back(d);
    }
    return out;
}

std::vector<nlohmann::json> ULLMToolRegistry::buildOpenAiToolsJson(const ToolFilter& filter) const
{
    std::vector<nlohmann::json> tools;
    for(const LLMToolDefinition& d : listForLlmApi(filter))
    {
        nlohmann::json t;
        t["type"] = "function";
        t["function"]["name"] = d.name;
        t["function"]["description"] = d.description;
        t["function"]["parameters"] = d.input_schema;
        tools.push_back(t);
    }
    return tools;
}

ToolGatewayResult ULLMToolRegistry::invokeHandler(const std::string& name,
                                                  const nlohmann::json& args) const
{
    ToolGatewayResult result;
    auto it = m_tools.find(name);
    if(it == m_tools.end())
    {
        result.ok = false;
        result.error_code = "ToolNotFound";
        result.message = name;
        return result;
    }
    if(!it->second.handler)
    {
        result.ok = false;
        result.error_code = "NoHandler";
        return result;
    }
    return it->second.handler(args);
}

} // namespace RDK::LLM
