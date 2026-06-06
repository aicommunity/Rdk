#ifndef RDK_ULLM_TOOL_REGISTRY_H
#define RDK_ULLM_TOOL_REGISTRY_H

#include <functional>
#include <map>
#include <vector>

#include "../LlmTypes.h"

namespace RDK::LLM {

using ToolHandler = std::function<ToolGatewayResult(const nlohmann::json&)>;

class ULLMToolRegistry {
public:
    void registerTool(const LLMToolDefinition& def, ToolHandler handler);
    const LLMToolDefinition* find(const std::string& name) const;
    std::vector<LLMToolDefinition> listForLlmApi(const ToolFilter& filter) const;
    std::vector<nlohmann::json> buildOpenAiToolsJson(const ToolFilter& filter) const;
    ToolGatewayResult invokeHandler(const std::string& name, const nlohmann::json& args) const;

private:
    struct Entry {
        LLMToolDefinition def;
        ToolHandler handler;
    };
    std::map<std::string, Entry> m_tools;
};

} // namespace RDK::LLM

#endif
