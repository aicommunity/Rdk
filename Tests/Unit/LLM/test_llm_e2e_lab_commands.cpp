#include <gtest/gtest.h>

#include <cstdlib>

#include "llm_e2e_lab_harness.h"
#include "Orchestrator/ULLMIntentParser.h"

using namespace RDK::LLM;
using namespace RDK::LLM::E2eLab;

namespace {

class E2eLabCommands : public ::testing::Test {
protected:
    void SetUp() override
    {
        skipIfLabOllamaUnreachable();
        setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    }
};

} // namespace

TEST_F(E2eLabCommands, OrchestratorQueryUnderstandsConfigurationTools)
{
    Harness h;
    LLMSessionContext session = defaultSession(false);
    const std::string sid = "e2e-query-tools";

    const LLMRequestEnvelope req =
        makeRequest(h, sid,
                    "List the RDK LLM tool names related to configuration lifecycle "
                    "(create, load, save, close, validate). Reply in one short paragraph.",
                    session);

    const LLMFinalResponse resp = h.orchestrator->handleUserMessage(req);
    if(!resp.ok && resp.error.find("404") != std::string::npos)
        GTEST_SKIP() << resp.error;

    ASSERT_TRUE(resp.ok) << resp.error;
    EXPECT_FALSE(resp.text.empty());
    const std::string lower = [&]() {
        std::string s = resp.text;
        for(char& c : s)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }();
    EXPECT_TRUE(lower.find("configuration") != std::string::npos
                || lower.find("validate") != std::string::npos
                || lower.find("load") != std::string::npos)
        << resp.text;
}

TEST_F(E2eLabCommands, OrchestratorMutateInvokesValidateConfigurationTool)
{
    Harness h;
    LLMSessionContext session = defaultSession(false);
    const std::string sid = "e2e-validate-tool";
    const std::string ini = sampleValidConfigurationIni();

    const std::string prompt =
        "Tool-only task: invoke exactly one function validate_configuration with JSON arguments "
        "{\"configuration_path\": \"" +
        ini + "\"}. Do not call any other tools. Do not explain in prose before calling the tool.";

    const LLMRequestEnvelope req = makeRequest(h, sid, prompt, session);
    LLMFinalResponse resp = h.orchestrator->handleUserMessage(req);

    if(!resp.ok && (resp.error.find("404") != std::string::npos
                    || resp.error.find("tool") != std::string::npos))
        GTEST_SKIP() << resp.error;

    ASSERT_TRUE(resp.ok) << resp.error;

    if(resp.pending_confirmation)
    {
        resp = h.orchestrator->confirmPending(sid, resp.pending_confirmation_id);
        ASSERT_TRUE(resp.ok) << resp.error;
    }

    EXPECT_TRUE(conversationUsedTool(h.store, sid, "validate_configuration"))
        << "Expected validate_configuration in conversation tool messages";

    nlohmann::json tool_result;
    ASSERT_TRUE(findToolResultJson(h.store, sid, "validate_configuration", tool_result));

    if(tool_result.contains("is_valid"))
        EXPECT_TRUE(tool_result["is_valid"].is_boolean());
    else if(tool_result.contains("status"))
        EXPECT_TRUE(tool_result["status"].is_object());
    else if(!tool_result.empty())
        EXPECT_TRUE(tool_result.contains("message") || tool_result.contains("error_code")
                    || tool_result.contains("ok"))
            << tool_result.dump();
}

TEST_F(E2eLabCommands, OrchestratorMutateInvokesListRegisteredClasses)
{
    Harness h;
    LLMSessionContext session = defaultSession(false);
    const std::string sid = "e2e-list-classes";

    const std::string prompt =
        "Tool-only: call exactly one tool list_registered_classes with empty arguments {}. "
        "No other tools.";

    const LLMRequestEnvelope req = makeRequest(h, sid, prompt, session);
    LLMFinalResponse resp = h.orchestrator->handleUserMessage(req);

    if(!resp.ok && (resp.error.find("404") != std::string::npos
                    || resp.error.find("tool") != std::string::npos))
        GTEST_SKIP() << resp.error;

    ASSERT_TRUE(resp.ok) << resp.error;

    if(resp.pending_confirmation)
        resp = h.orchestrator->confirmPending(sid, resp.pending_confirmation_id);

    EXPECT_TRUE(conversationUsedTool(h.store, sid, "list_registered_classes")
                || !resp.text.empty())
        << "Model should call list_registered_classes or answer with class list";
}

TEST_F(E2eLabCommands, OrchestratorMutateLoadConfigurationFlow)
{
    Harness h;
    LLMSessionContext session = defaultSession(false);
    const std::string sid = "e2e-load-config";
    const std::string ini = sampleValidConfigurationIni();

    const std::string prompt =
        "Tool-only: call exactly one tool load_configuration with arguments "
        "{\"configuration_path\": \"" +
        ini +
        "\", \"if_open_project\": \"close\"}. No other tools.";

    const LLMRequestEnvelope req = makeRequest(h, sid, prompt, session);
    LLMFinalResponse resp = h.orchestrator->handleUserMessage(req);

    if(!resp.ok && (resp.error.find("404") != std::string::npos
                    || resp.error.find("tool") != std::string::npos))
        GTEST_SKIP() << resp.error;

    if(resp.pending_confirmation && !resp.pending_confirmation_id.empty())
    {
        const ConversationState& state = h.store.getOrCreate(sid);
        ASSERT_TRUE(state.pending);
        const std::string& tool = state.pending->request.tool_name;
        EXPECT_TRUE(tool == "load_configuration" || tool == "load_project") << tool;

        resp = h.orchestrator->confirmPending(sid, resp.pending_confirmation_id);
        if(!resp.ok)
        {
            EXPECT_TRUE(resp.error.find("Application") != std::string::npos
                        || resp.error.find("not available") != std::string::npos)
                << resp.error;
        }
        return;
    }

    const bool requested_load =
        conversationAssistantRequestedTool(h.store, sid, "load_configuration")
        || conversationAssistantRequestedTool(h.store, sid, "load_project");
    const bool used_load = conversationUsedTool(h.store, sid, "load_configuration")
                          || conversationUsedTool(h.store, sid, "load_project");
    EXPECT_TRUE(requested_load || used_load)
        << "Expected load_configuration in assistant tool_calls or tool results";

    if(!resp.ok)
    {
        EXPECT_TRUE(resp.error.find("Application") != std::string::npos
                    || resp.error.find("not available") != std::string::npos)
            << resp.error;
    }
}

TEST_F(E2eLabCommands, OrchestratorNaturalLanguageValidateIntent)
{
    ULLMIntentParser parser;
    const std::string ini = sampleValidConfigurationIni();
    const std::string phrase = "проверь конфигурацию по пути " + ini;
    const IntentParseResult intent = parser.parseDetailed(phrase);
    EXPECT_TRUE(intent.kind == LLMIntentKind::Query || intent.kind == LLMIntentKind::Mutate);

    Harness h;
    LLMSessionContext session = defaultSession(false);
    const std::string sid = "e2e-nl-validate";

    const LLMRequestEnvelope req = makeRequest(h, sid, phrase, session);
    LLMFinalResponse resp = h.orchestrator->handleUserMessage(req);

    if(!resp.ok && resp.error.find("404") != std::string::npos)
        GTEST_SKIP() << resp.error;

    ASSERT_TRUE(resp.ok) << resp.error;

    if(resp.pending_confirmation)
        resp = h.orchestrator->confirmPending(sid, resp.pending_confirmation_id);

    const bool validate_used = conversationUsedTool(h.store, sid, "validate_configuration");
    const bool mentioned = resp.text.find("valid") != std::string::npos
                           || resp.text.find("is_valid") != std::string::npos;
    EXPECT_TRUE(validate_used || mentioned)
        << "Natural-language validate should invoke tool or report validation result";
}

TEST_F(E2eLabCommands, GatewayValidateConfigurationDirectCommand)
{
    Harness h;
    const std::string ini = sampleValidConfigurationIni();

    ToolInvokeRequest req;
    req.trace_id = "e2e-direct";
    req.tool_name = "validate_configuration";
    req.arguments = {{"configuration_path", ini}};
    req.session = defaultSession(false);
    req.confirmed = true;

    const ToolGatewayResult tr = h.gateway.invoke(req);
    if(!tr.ok)
    {
        EXPECT_TRUE(tr.message.find("Application") != std::string::npos
                    || tr.error_code == "DomainError")
            << tr.message;
        return;
    }
    if(tr.result.contains("is_valid"))
        EXPECT_TRUE(tr.result["is_valid"].is_boolean());
}
