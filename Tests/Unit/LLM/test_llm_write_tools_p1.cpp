#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>

#include "Domain/URdkDomainAccess.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Orchestrator/ULLMToolFilterBuilder.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Policy/ULLMWriteToolPolicy.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace {

std::string fixturePath(const char* name)
{
    return std::string(CMAKE_SOURCE_DIR) + "/Rdk/Tests/Fixtures/LLM/tools/" + name;
}

nlohmann::json loadJsonFixture(const char* name)
{
    std::ifstream in(fixturePath(name));
    nlohmann::json j;
    in >> j;
    return j;
}

bool filterAllows(const ToolFilter& filter, const char* tool)
{
    if(!filter.allowed_tool_names)
        return false;
    return filter.allowed_tool_names->count(tool) > 0;
}

struct GatewayHarness {
    ULLMToolRegistry registry;
    ULLMPolicyEngine policy;
    URdkDomainAccess domain;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway;

    GatewayHarness()
        : domain(nullptr)
        , gateway(registry, policy, domain, GetAuditLog(), idem, validator)
    {
        RegisterCoreRdkTools(registry, domain, nullptr);
    }
};

} // namespace

TEST(LLMWriteToolsP1, MutateFilterIncludesCoreWriteTools)
{
    const ToolFilter filter =
        buildToolFilter(LLMIntentKind::Mutate, true, ConfigurationLifecycleAction::None);
    EXPECT_TRUE(filter.include_write);
    EXPECT_TRUE(filterAllows(filter, "add_component"));
    EXPECT_TRUE(filterAllows(filter, "set_property"));
    EXPECT_TRUE(filterAllows(filter, "remove_component"));
    EXPECT_TRUE(filterAllows(filter, "connect_components"));
    EXPECT_TRUE(filterAllows(filter, "disconnect_components"));
    EXPECT_TRUE(filterAllows(filter, "list_model_links"));
    EXPECT_TRUE(filterAllows(filter, "get_component_ports"));
    EXPECT_TRUE(filterAllows(filter, "start_channel_calculation"));
    EXPECT_TRUE(filterAllows(filter, "run_n_steps"));
    EXPECT_TRUE(filterAllows(filter, "add_channel"));
    EXPECT_TRUE(filterAllows(filter, "delete_channel"));
    EXPECT_TRUE(filterAllows(filter, "clone_channel"));
    EXPECT_TRUE(filterAllows(filter, "clone_component"));
    EXPECT_TRUE(filterAllows(filter, "move_component"));
    EXPECT_TRUE(filterAllows(filter, "rename_component"));
    EXPECT_TRUE(filterAllows(filter, "reorder_component"));
    EXPECT_TRUE(filterAllows(filter, "export_component"));
    EXPECT_TRUE(filterAllows(filter, "import_component"));
    EXPECT_TRUE(filterAllows(filter, "calculate_component"));
    EXPECT_TRUE(filterAllows(filter, "reset_component"));
    EXPECT_TRUE(filterAllows(filter, "default_component"));
    EXPECT_TRUE(filterAllows(filter, "select_component"));
    EXPECT_TRUE(filterAllows(filter, "save_configuration"));
    EXPECT_TRUE(filterAllows(filter, "save_configuration_as"));
    EXPECT_TRUE(filterAllows(filter, "validate_configuration"));
}

TEST(LLMWriteToolsP1, QueryFilterIncludesLinkInspectTools)
{
    const ToolFilter filter =
        buildToolFilter(LLMIntentKind::Query, false, ConfigurationLifecycleAction::None);
    EXPECT_TRUE(filterAllows(filter, "list_model_links"));
    EXPECT_TRUE(filterAllows(filter, "get_component_ports"));
    EXPECT_TRUE(filterAllows(filter, "get_net_snapshot"));
}

TEST(LLMWriteToolsP1, LowRiskPropertySkipsConfirmation)
{
    EXPECT_TRUE(isLowRiskPropertyName("Comment"));
    EXPECT_TRUE(isLowRiskPropertyName("description"));
    EXPECT_FALSE(isLowRiskPropertyName("ClassName"));
    EXPECT_FALSE(isLowRiskPropertyName(""));

    const nlohmann::json args = loadJsonFixture("set_property_comment.json");
    EXPECT_TRUE(isConfirmationExemptWriteTool("set_property", args));
    EXPECT_FALSE(isConfirmationExemptWriteTool("add_component", args));
}

TEST(LLMWriteToolsP1, SetPropertyCommentInvokesWithoutHitl)
{
    GatewayHarness h;
    ToolInvokeRequest req;
    req.tool_name = "set_property";
    req.arguments = loadJsonFixture("set_property_comment.json");
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = 1;

    const ToolGatewayResult r = h.gateway.invoke(req);
    EXPECT_FALSE(r.pending_confirmation);
    EXPECT_TRUE(r.confirmation_id.empty());
}

TEST(LLMWriteToolsP1, SetPropertyClassNameRequiresConfirmation)
{
    GatewayHarness h;
    ToolInvokeRequest req;
    req.tool_name = "set_property";
    req.arguments = {{"long_name", "Model.X"},
                     {"property_name", "ClassName"},
                     {"value", "Other"}};
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = 1;

    const ToolGatewayResult r = h.gateway.invoke(req);
    EXPECT_TRUE(r.pending_confirmation);
}

TEST(LLMWriteToolsP1, AddComponentSchemaRequiresParentAndNames)
{
    GatewayHarness h;
    ULLMToolArgumentValidator validator;
    const LLMToolDefinition* def = h.registry.find("add_component");
    ASSERT_NE(def, nullptr);
    std::string err;
    EXPECT_FALSE(validator.validate(nlohmann::json{{"class_name", "Model"}}, def->input_schema, err));
    EXPECT_TRUE(validator.validate(loadJsonFixture("add_component_model.json"), def->input_schema, err))
        << err;
}

TEST(LLMWriteToolsP1, AddComponentHandlerMissingFieldsReturnsErrorNotThrow)
{
    GatewayHarness h;
    const ToolGatewayResult r =
        h.registry.invokeHandler("add_component", nlohmann::json{{"class_name", "NSPNeuron"}});
    EXPECT_FALSE(r.ok);
    EXPECT_EQ(r.error_code, "ARGS_REQUIRED");
    EXPECT_NE(r.message.find("parent_long_name"), std::string::npos);
}

TEST(LLMWriteToolsP1, AddComponentMockOrchestratorHitl)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");
    ::unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");
    ::setenv("NMSDK_LLM_INPUT_ENSEMBLE", "0", 1);

    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    LLMToolCall call;
    call.id = "call_add";
    call.name = "add_component";
    call.arguments = loadJsonFixture("add_component_model.json");
    mock.tool_calls.push_back(call);
    provider.enqueue(mock);
    provider.enqueue(mock);

    GatewayHarness gh;
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, gh.registry, gh.gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "p1-add";
    req.trace_id = "t-add";
    req.user_text = "add MatrixSource to the model";
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = 1;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.pending_confirmation);
    EXPECT_FALSE(resp.pending_confirmation_id.empty());
}

TEST(LLMWriteToolsP1, ConnectComponentsFixtureParses)
{
    GatewayHarness h;
    const LLMToolDefinition* def = h.registry.find("connect_components");
    ASSERT_NE(def, nullptr);
    std::string err;
    EXPECT_TRUE(
        h.validator.validate(loadJsonFixture("connect_components_ports.json"), def->input_schema, err))
        << err;
}

TEST(LLMWriteToolsP1, CopyConfigurationRequiresPathAndConfirmation)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    GatewayHarness h;
    ToolInvokeRequest req;
    req.tool_name = "copy_configuration";
    req.arguments = {{"destination_directory", "/tmp/nmsdk_llm_copy_dest"}};
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = 1;
    req.confirmed = false;

    const ToolGatewayResult r = h.gateway.invoke(req);
    EXPECT_TRUE(r.pending_confirmation);
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMWriteToolsP1, RemoveComponentRequiresConfirmation)
{
    GatewayHarness h;
    ToolInvokeRequest req;
    req.tool_name = "remove_component";
    req.arguments = {{"long_name", "PGenerator"}, {"channel_index", 0}};
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.user_id = 1;

    const ToolGatewayResult r = h.gateway.invoke(req);
    EXPECT_TRUE(r.pending_confirmation);
}

TEST(LLMWriteToolsP1, SaveConfigurationDeniedWhenAllowSaveFalse)
{
    GatewayHarness h;
    ToolInvokeRequest req;
    req.tool_name = "save_configuration";
    req.arguments = nlohmann::json::object();
    req.session.project_loaded = true;
    req.session.llm_write_enabled = true;
    req.session.allow_save = false;
    req.session.user_id = 1;
    req.confirmed = true;

    const ToolGatewayResult r = h.gateway.invoke(req);
    EXPECT_FALSE(r.ok);
    EXPECT_FALSE(r.pending_confirmation);
}

TEST(LLMWriteToolsP1, DisconnectComponentsInMutateFilter)
{
    const ToolFilter filter =
        buildToolFilter(LLMIntentKind::Mutate, true, ConfigurationLifecycleAction::None);
    EXPECT_TRUE(filterAllows(filter, "disconnect_components"));
}

TEST(LLMWriteToolsP1, ChannelCalcRegisteredWithSchema)
{
    GatewayHarness h;
    for(const char* name :
        {"start_channel_calculation", "pause_channel_calculation", "reset_channel_calculation",
         "step_channel_calculation", "run_n_steps", "add_channel", "delete_channel",
         "clone_channel"})
    {
        const LLMToolDefinition* def = h.registry.find(name);
        ASSERT_NE(def, nullptr) << name;
        EXPECT_TRUE(def->requires_confirmation);
        EXPECT_TRUE(def->requires_project_loaded);
    }
}

TEST(LLMWriteToolsP1, GraphEditAndComponentEnvToolsRegistered)
{
    GatewayHarness h;
    for(const char* name : {"clone_component", "move_component", "rename_component",
                            "reorder_component", "export_component", "import_component",
                            "calculate_component", "reset_component", "default_component",
                            "select_component"})
    {
        const LLMToolDefinition* def = h.registry.find(name);
        ASSERT_NE(def, nullptr) << name;
        EXPECT_EQ(def->kind, LLMToolKind::Write) << name;
    }
    const LLMToolDefinition* select = h.registry.find("select_component");
    ASSERT_NE(select, nullptr);
    EXPECT_FALSE(select->requires_confirmation);

    const LLMToolDefinition* run_n = h.registry.find("run_n_steps");
    ASSERT_NE(run_n, nullptr);
    EXPECT_TRUE(run_n->input_schema.contains("required"));
    EXPECT_TRUE(run_n->input_schema["properties"].contains("steps"));
}
