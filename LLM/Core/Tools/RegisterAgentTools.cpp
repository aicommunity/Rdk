#include "RegisterAgentTools.h"

#include "../LlmPublicApi.h"
#include "../Orchestrator/ULLMSubagentRunner.h"
#include "../Session/ULLMConversationStore.h"
#include "ULLMEmbeddingToolRouter.h"
#include "ULLMSearchTools.h"
#include "ULLMToolGateway.h"
#include "ULLMToolRegistry.h"

namespace RDK::LLM {

namespace {

constexpr int kDefaultSubagentMaxRounds = 4;
constexpr int kSessionSubagentRoundBudget = 12;

ILLMProvider* g_spawn_provider = nullptr;
ULLMToolRegistry* g_spawn_registry = nullptr;
ULLMToolGateway* g_spawn_gateway = nullptr;

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

ToolGatewayResult handleSpawnExploreSubagent(const nlohmann::json& args)
{
    ToolGatewayResult r;
    if(!g_spawn_provider || !g_spawn_registry || !g_spawn_gateway)
    {
        r.ok = false;
        r.error_code = "SUBAGENT_NOT_BOUND";
        r.message = "spawn_explore_subagent is not bound (LLMServices not ready).";
        return r;
    }

    const ToolInvokeRequest* active = activeToolInvokeRequest();
    LLMSessionContext session;
    std::string trace_id;
    std::string session_id;
    if(active)
    {
        session = active->session;
        trace_id = active->trace_id;
        session_id = active->session.session_id;
    }

    ConversationState* state = nullptr;
    if(LLMServices::instance().isInitialized() && !session_id.empty())
        state = LLMServices::instance().mutableConversationState(session_id);

    if(state && state->subagent_rounds_used >= kSessionSubagentRoundBudget)
    {
        r.ok = false;
        r.error_code = "SUBAGENT_BUDGET_EXCEEDED";
        r.message = "Session subagent round budget exceeded.";
        r.result = {{"subagent_rounds_used", state->subagent_rounds_used},
                    {"budget", kSessionSubagentRoundBudget}};
        return r;
    }

    SubagentRunRequest sreq;
    sreq.task = args.value("task", std::string());
    sreq.profile = args.value("profile", std::string("explore"));
    sreq.max_rounds = args.value("max_rounds", kDefaultSubagentMaxRounds);
    if(sreq.max_rounds <= 0)
        sreq.max_rounds = kDefaultSubagentMaxRounds;
    if(sreq.max_rounds > kDefaultSubagentMaxRounds)
        sreq.max_rounds = kDefaultSubagentMaxRounds;

    ULLMSubagentRunner runner(*g_spawn_provider, *g_spawn_registry, *g_spawn_gateway);
    const SubagentRunResult sres = runner.runExplore(sreq, trace_id, session_id, session);
    if(state)
        state->subagent_rounds_used += sres.rounds_used;

    r.ok = sres.ok;
    r.result = {{"summary", sres.summary},
                {"profile", sreq.profile.empty() ? "explore" : sreq.profile},
                {"rounds_used", sres.rounds_used},
                {"subagent_rounds_used", state ? state->subagent_rounds_used : sres.rounds_used}};
    if(!sres.ok)
    {
        r.error_code = "SUBAGENT_FAILED";
        r.message = sres.summary;
    }
    return r;
}

} // namespace

void bindSpawnExploreSubagent(ILLMProvider& provider, ULLMToolRegistry& registry,
                              ULLMToolGateway& gateway)
{
    g_spawn_provider = &provider;
    g_spawn_registry = &registry;
    g_spawn_gateway = &gateway;
}

void RegisterAgentTools(ULLMToolRegistry& registry)
{
    ULLMToolRegistry* registry_ptr = &registry;
    registry.registerTool(
        agentToolDef("ask_user",
                     "Use when required arguments or user intent are ambiguous and you cannot "
                     "safely proceed. Pauses the turn until the user replies. "
                     "Do not use for ordinary status answers — reply in text instead.",
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
                     "Use when the user needs a multi-step write plan for review before execution. "
                     "Does not execute writes. Do not use for single-tool read queries.",
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
                     "Search deferred/long-tail tools by keyword. Returned tools are added to the "
                     "available tool set for subsequent reasoning rounds (progressive disclosure). "
                     "Use when the needed capability is not in the current tool list.",
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
            r.result = enrichSearchToolsPayload(found, *registry_ptr);
            return r;
        });

    registry.registerTool(
        agentToolDef("spawn_explore_subagent",
                     "Spawn a read-only explore subagent with isolated tool budget.",
                     {{"type", "object"},
                      {"required", {"task"}},
                      {"properties",
                       {{"task", {{"type", "string"}}},
                        {"profile", {{"type", "string"}}},
                        {"max_rounds", {{"type", "integer"}}}}},
                      {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return handleSpawnExploreSubagent(args);
        });
}

} // namespace RDK::LLM
