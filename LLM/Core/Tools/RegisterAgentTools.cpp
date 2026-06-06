#include "RegisterAgentTools.h"

#include "ULLMEmbeddingToolRouter.h"
#include "ULLMToolRegistry.h"

namespace RDK::LLM {

namespace {

LLMToolDefinition agentToolDef(const std::string& name, const std::string& desc, nlohmann::json input)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    d.requires_confirmation = false;
    return d;
}

} // namespace

void RegisterAgentTools(ULLMToolRegistry& registry)
{
    ULLMToolRegistry* registry_ptr = &registry;
    registry.registerTool(
        agentToolDef("ask_user",
                     "Ask the user a clarifying question. Orchestrator pauses until the user replies.",
                     {{"type", "object"},
                      {"required", {"question"}},
                      {"properties",
                       {{"question", {{"type", "string"}}},
                        {"choices", {{"type", "array"}, {"items", {{"type", "string"}}}}},
                        {"allow_free_text", {{"type", "boolean"}}}}},
                      {"additionalProperties", false}}),
        [](const nlohmann::json&) -> ToolGatewayResult {
            ToolGatewayResult r;
            r.ok = true;
            r.message = "ask_user_deferred_to_orchestrator";
            return r;
        });

    registry.registerTool(
        agentToolDef("propose_plan",
                     "Propose a multi-step plan for user review (does not execute writes).",
                     {{"type", "object"},
                      {"required", {"goal", "steps"}},
                      {"properties",
                       {{"goal", {{"type", "string"}}},
                        {"steps", {{"type", "array"}}}}},
                      {"additionalProperties", false}}),
        [](const nlohmann::json&) -> ToolGatewayResult {
            ToolGatewayResult r;
            r.ok = true;
            r.message = "propose_plan_deferred_to_orchestrator";
            return r;
        });

    registry.registerTool(
        agentToolDef("search_tools",
                     "Search deferred/long-tail tools by keyword (embedding router).",
                     {{"type", "object"},
                      {"required", {"query"}},
                      {"properties", {{"query", {{"type", "string"}}}, {"top_k", {{"type", "integer"}}}}},
                      {"additionalProperties", false}}),
        [registry_ptr](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            const std::string query = args.value("query", "");
            const int top_k = args.value("top_k", 12);
            const SearchToolsResult found = searchToolsHybrid(*registry_ptr, query, top_k);
            r.ok = true;
            r.result = {{"query", found.query},
                        {"index_version", found.index_version},
                        {"tools", found.tools}};
            return r;
        });

    registry.registerTool(
        agentToolDef("spawn_explore_subagent",
                     "Spawn a read-only explore subagent with isolated tool budget.",
                     {{"type", "object"},
                      {"required", {"task"}},
                      {"properties", {{"task", {{"type", "string"}}}}},
                      {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            ToolGatewayResult r;
            r.ok = true;
            r.result = {{"task", args.value("task", "")}, {"status", "subagent_stub"}};
            return r;
        });
}

} // namespace RDK::LLM
