#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>

#include "LlmModuleInit.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Settings/ULLMProviderCatalog.h"
#include "llm_test_ollama_lab.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMSearchTools.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

namespace {

struct OrchFixture {
    ULLMMockProvider provider;
    ULLMToolRegistry registry;
    ULLMPolicyEngine policy;
    URdkDomainAccess domain{nullptr};
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway{registry, policy, domain, audit, idem, validator};
    ULLMConversationStore store;

    OrchFixture()
    {
        RegisterCoreRdkTools(registry, domain, nullptr);
        RegisterAgentTools(registry);
        provider.setSupportsThinking(true);
        ::unsetenv("NMSDK_LLM_INTENT_LLM");
        ::unsetenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE");
        ::setenv("NMSDK_LLM_INPUT_ENSEMBLE", "0", 1);
    }

    ULLMAgentOrchestrator makeOrch()
    {
        return ULLMAgentOrchestrator(provider, registry, gateway, store);
    }
};

bool containsTool(const std::vector<std::string>& names, const std::string& want)
{
    return std::find(names.begin(), names.end(), want) != names.end();
}

} // namespace

TEST(LLMOrchestratorThinking, PreservesThinkingOnToolLoopRound)
{
    OrchFixture fx;
    LLMCompletionResult round1;
    round1.ok = true;
    round1.thinking = "need graph before answering";
    LLMToolCall call;
    call.id = "tc1";
    call.name = "get_net_snapshot";
    call.arguments = nlohmann::json::object();
    round1.tool_calls.push_back(call);
    fx.provider.enqueue(round1);

    LLMCompletionResult round2;
    round2.ok = true;
    round2.text = "Graph inspected.";
    fx.provider.enqueue(round2);

    ULLMAgentOrchestrator orch = fx.makeOrch();
    LLMRequestEnvelope req;
    req.session_id = "think-preserve";
    req.trace_id = "t-preserve";
    req.user_text = "what components are in the model?";
    req.session.project_loaded = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    ASSERT_GE(fx.provider.invokeCount(), 2u);

    bool saw_thinking = false;
    for(const LLMMessage& m : fx.provider.lastMessages())
    {
        if(m.role == LLMMessage::Role::Assistant && m.thinking == "need graph before answering")
            saw_thinking = true;
    }
    EXPECT_TRUE(saw_thinking) << "assistant tool-call turn must keep unmodified thinking";
}

TEST(LLMOrchestratorThinking, NoForcedToolChoiceWhenThinkingOn)
{
    OrchFixture fx;
    ::setenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE", "1", 1);

    LLMCompletionResult mock;
    mock.ok = true;
    mock.thinking = "consider create";
    mock.text = "I will create a configuration.";
    fx.provider.enqueue(mock);
    fx.provider.enqueue(mock);

    ULLMAgentOrchestrator orch = fx.makeOrch();
    LLMRequestEnvelope req;
    req.session_id = "think-no-force";
    req.trace_id = "t-noforce";
    req.user_text = "create configuration";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    (void)orch.handleUserMessage(req);
    EXPECT_EQ(fx.provider.lastOpts().think_mode, LLMThinkMode::On);
    EXPECT_FALSE(fx.provider.lastOpts().tool_choice.has_value())
        << "DD-THINK-003: no forced tool_choice while thinking";

    ::unsetenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE");
}

TEST(LLMOrchestratorThinking, SearchToolsExpandsAllowlistNextRound)
{
    OrchFixture fx;

    // Sanity: lexical search finds the exact tool name.
    {
        const SearchToolsResult found =
            searchToolsByQuery(fx.registry, "list_model_links", 8);
        EXPECT_TRUE(containsTool(found.tools, "list_model_links"));
    }

    LLMCompletionResult round1;
    round1.ok = true;
    round1.thinking = "need list_model_links";
    LLMToolCall call;
    call.id = "tc-search";
    call.name = "search_tools";
    call.arguments = {{"query", "list_model_links"}, {"top_k", 8}};
    round1.tool_calls.push_back(call);
    fx.provider.enqueue(round1);

    LLMCompletionResult round2;
    round2.ok = true;
    round2.text = "Found the tool.";
    fx.provider.enqueue(round2);

    ULLMAgentOrchestrator orch = fx.makeOrch();
    LLMRequestEnvelope req;
    req.session_id = "think-expand";
    req.trace_id = "t-expand";
    req.user_text = "show me model links between components";
    req.session.project_loaded = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok) << resp.error << " / " << resp.text;
    ASSERT_GE(fx.provider.invokeCount(), 2u);
    const auto names = fx.provider.lastToolNames();
    std::string joined;
    for(const std::string& n : names)
    {
        if(!joined.empty())
            joined += ",";
        joined += n;
    }
    EXPECT_TRUE(containsTool(names, "list_model_links"))
        << "search_tools must expand allowlist for next round; tools=[" << joined << "]";
}

TEST(ProviderCatalog, ThinkingProfileIsNativeQwen3)
{
    const LLMProviderProfile* p = ULLMProviderCatalog::findById("ollama-thinking");
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->kind, LLMProviderKind::OllamaNative);
    // Catalog is built once; model is env override or default pin qwen3:14b.
    EXPECT_NE(p->model.find("qwen3"), std::string::npos) << p->model;
}

TEST(LabThinkingModel, DefaultPinIsQwen314b)
{
    const char* prev = std::getenv("NMSDK_LLM_OLLAMA_THINKING_MODEL");
    const std::string prev_val = prev ? prev : "";
    ::unsetenv("NMSDK_LLM_OLLAMA_THINKING_MODEL");
    EXPECT_EQ(RDK::LLM::Test::labOllamaThinkingModelName(), "qwen3:14b");
    if(!prev_val.empty())
        ::setenv("NMSDK_LLM_OLLAMA_THINKING_MODEL", prev_val.c_str(), 1);
}
