#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

#ifndef RDK_LLM_FREE_DIALOGUE_CORPUS_DIR
#define RDK_LLM_FREE_DIALOGUE_CORPUS_DIR "Rdk/Tests/Fixtures/LLM/free-dialogue-corpus"
#endif

namespace fs = std::filesystem;
using namespace RDK::LLM;

TEST(FreeDialogueCorpus, LoadsAtLeast30Scenarios)
{
    const fs::path root(RDK_LLM_FREE_DIALOGUE_CORPUS_DIR);
    ASSERT_TRUE(fs::is_directory(root)) << root;
    int count = 0;
    for(const auto& entry : fs::directory_iterator(root))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        EXPECT_TRUE(j.contains("id"));
        EXPECT_TRUE(j.contains("user_text"));
        ++count;
    }
    EXPECT_GE(count, 30);
}

TEST(FreeDialogueCorpus, ExpectsNoHardTaskPathFailFlag)
{
    const fs::path root(RDK_LLM_FREE_DIALOGUE_CORPUS_DIR);
    for(const auto& entry : fs::directory_iterator(root))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        if(j.contains("expect") && j["expect"].is_object())
            EXPECT_TRUE(j["expect"].value("no_hard_task_path_fail", false));
    }
}

TEST(FreeDialogueCorpus, MockOrchestratorHandlesCorpusPrompts)
{
    unsetenv("NMSDK_LLM_INTENT_LLM");
    unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");
    setenv("NMSDK_LLM_INPUT_ENSEMBLE", "0", 1);
    unsetenv("NMSDK_LLM_TASK_PATH_STRICT");

    const fs::path root(RDK_LLM_FREE_DIALOGUE_CORPUS_DIR);
    int ran = 0;
    for(const auto& entry : fs::directory_iterator(root))
    {
        if(entry.path().extension() != ".json")
            continue;
        if(ran >= 5)
            break;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        const std::string user_text = j.value("user_text", "");
        if(user_text.empty())
            continue;

        ULLMMockProvider provider;
        LLMCompletionResult mock;
        mock.ok = true;
        mock.text = "Free dialogue response.";
        provider.enqueue(mock);
        provider.enqueue(mock);

        ULLMToolRegistry registry;
        ULLMPolicyEngine policy;
        URdkDomainAccess domain(nullptr);
        ULLMAuditLog audit;
        ULLMIdempotencyStore idem;
        ULLMToolArgumentValidator validator;
        ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
        RegisterCoreRdkTools(registry, domain, nullptr);
        RegisterAgentTools(registry);
        ULLMConversationStore store;
        ULLMAgentOrchestrator orch(provider, registry, gateway, store);

        LLMRequestEnvelope req;
        req.session_id = "fd-" + j.value("id", "x");
        req.trace_id = "tr";
        req.user_text = user_text;
        req.session.project_loaded = true;

        const LLMFinalResponse resp = orch.handleUserMessage(req);
        EXPECT_TRUE(resp.ok) << j.value("id", "");
        ++ran;
    }
    EXPECT_GE(ran, 5);
}
