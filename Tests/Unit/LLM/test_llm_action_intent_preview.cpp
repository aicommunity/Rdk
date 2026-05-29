#include <gtest/gtest.h>

#include "Orchestrator/ULLMActionIntentPreview.h"
#include "Orchestrator/ULLMWriteToolExecution.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Providers/ULLMMockProvider.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMActionIntentPreview, AddComponentShowsScopeAndParent)
{
    LLMGuiContextSnapshot gui;
    gui.diagram_scope_long_name = "Model/ZoneA";
    gui.current_component_long_name = "Model/ZoneA/Neuron1";
    gui.channel_index = 2;
    gui.project_xml_path = "/tmp/demo/project.ini";

    LLMSessionContext session;
    session.project_loaded = true;
    session.active_channel_index = 2;

    nlohmann::json args = {{"class_name", "PulseNeuron"},
                           {"parent_long_name", "Model/ZoneA"},
                           {"short_name", "N1"}};

    const std::string preview =
        formatActionIntentPreview("add_component", args, gui, session, "en");
    EXPECT_NE(preview.find("Model/ZoneA"), std::string::npos);
    EXPECT_NE(preview.find("PulseNeuron"), std::string::npos);
    EXPECT_NE(preview.find("Add component"), std::string::npos);
    EXPECT_NE(preview.find("project.ini"), std::string::npos);
}

TEST(LLMActionIntentPreview, ConnectShowsEndpointsRussian)
{
    LLMGuiContextSnapshot gui;
    gui.diagram_scope_long_name = "Model";
    nlohmann::json args = {{"from_long_name", "Model/A"},
                           {"to_long_name", "Model/B"},
                           {"from_port", "Out"},
                           {"to_port", "In"}};

    LinkPreviewContext link_ctx;
    link_ctx.catalog_lines = {"NSPNeuron → LTZone (ExcSynapse1)"};

    const std::string preview =
        formatActionIntentPreview("connect_components", args, gui, LLMSessionContext{}, "ru",
                                  &link_ctx);
    EXPECT_NE(preview.find("Соединить"), std::string::npos);
    EXPECT_NE(preview.find("Model/A"), std::string::npos);
    EXPECT_NE(preview.find("каталог"), std::string::npos);
    EXPECT_NE(preview.find("ExcSynapse1"), std::string::npos);
}

TEST(LLMActionIntentPreview, OpenRecentShowsActionLabel)
{
    const std::string preview = formatActionIntentPreview(
        "open_recent_configuration", nlohmann::json::object(), LLMGuiContextSnapshot{},
        LLMSessionContext{}, "en");
    EXPECT_NE(preview.find("Open recent"), std::string::npos);
}

TEST(LLMWriteToolExecution, HitlSkipsGatewayInvoke)
{
    ULLMToolRegistry registry;
    int invoke_count = 0;
    LLMToolDefinition def;
    def.name = "test_write_preview";
    def.kind = LLMToolKind::Write;
    def.requires_confirmation = true;
    def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(def, [&](const nlohmann::json&) {
        ++invoke_count;
        ToolGatewayResult r;
        r.ok = true;
        r.result["applied"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMMockProvider provider;
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    ConversationState& state = store.getOrCreate("hitl-preview");
    WriteToolExecutionRequest wreq;
    wreq.session_id = "hitl-preview";
    wreq.trace_id = "t1";
    wreq.tool_name = "test_write_preview";
    wreq.arguments = nlohmann::json::object();
    wreq.session.project_loaded = true;
    wreq.session.llm_write_enabled = true;
    wreq.user_lang = "en";

    const WriteToolExecutionResult result =
        executeWriteWithPreviewAndVerify(orch, state, wreq);

    EXPECT_TRUE(result.needs_hitl);
    EXPECT_TRUE(result.gateway.pending_confirmation);
    EXPECT_EQ(invoke_count, 0);
    EXPECT_NE(result.preview_text.find("test_write_preview"), std::string::npos);
    EXPECT_NE(result.outcome_text.find("Apply"), std::string::npos);
}
