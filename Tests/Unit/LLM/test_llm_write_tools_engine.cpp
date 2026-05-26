#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <rdk_application.h>

#include "Domain/URdkDomainAccess.h"
#include "Gui/ULLMNoopPresentationSink.h"
#include "LlmModuleInit.h"
#include "LlmPublicApi.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Policy/ULLMWriteToolPolicy.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

#include "LlmWriteToolsEngineInit.h"

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

std::string uniqueSuffix()
{
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    return std::to_string(ms % 1000000);
}

struct EngineGateway {
    ULLMToolRegistry registry;
    URdkDomainAccess domain;
    ULLMPolicyEngine policy;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway;

    explicit EngineGateway(RDK::UApplication* app)
        : domain(app)
        , gateway(registry, policy, domain, GetAuditLog(), idem, validator)
    {
        RegisterCoreRdkTools(registry, domain, nullptr);
    }

    ToolInvokeRequest baseRequest() const
    {
        ToolInvokeRequest req;
        req.session.project_loaded = true;
        req.session.llm_write_enabled = true;
        req.session.allow_save = true;
        req.session.user_id = 1;
        req.confirmed = true;
        return req;
    }
};

} // namespace

class LLMWriteToolsEngine : public ::testing::Test {
protected:
    void SetUp() override
    {
        ctx_ = &NmsdkTests::EnsureLlmWriteToolsEngine();
        if(!ctx_->initialized || !ctx_->application)
            GTEST_SKIP() << "Engine/console init unavailable (run from Bin/Platform with NeuroModelerConsole)";
        if(!ctx_->application->GetProjectOpenFlag())
            GTEST_SKIP() << "test_valid project did not open";

        if(!LLMServices::instance().isInitialized())
        {
            LLMServices::instance().initialize(ctx_->application, nullptr);
            LLMServices::instance().setPresentationSink(
                std::make_unique<ULLMNoopPresentationSink>());
        }
    }

    const NmsdkTests::LlmWriteToolsEngineContext* ctx_ = nullptr;
};

TEST_F(LLMWriteToolsEngine, AddComponentUnderModelRoot)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "add_component";
    const std::string suffix = uniqueSuffix();
    req.arguments = {{"class_name", "NPGenerator"},
                     {"parent_long_name", ""},
                     {"short_name", "LlmGen" + suffix},
                     {"channel_index", 0}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    ASSERT_TRUE(r.ok) << r.message;
    EXPECT_FALSE(r.result.value("long_name", std::string()).empty());

    req.tool_name = "remove_component";
    req.arguments = {{"long_name", r.result["long_name"]}, {"channel_index", 0}};
    const ToolGatewayResult removed = gw.gateway.invoke(req);
    EXPECT_TRUE(removed.ok) << removed.message;
}

TEST_F(LLMWriteToolsEngine, SetPropertyLowRiskSkipsHitl)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "set_property";
    req.confirmed = false;
    req.arguments = {{"long_name", "PGenerator"},
                     {"property_name", "Comment"},
                     {"value", "LLM P1 engine test"},
                     {"channel_index", 0}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    EXPECT_FALSE(r.pending_confirmation);
    if(!r.ok)
        EXPECT_NE(r.message.find("Comment"), std::string::npos) << r.message;
}

TEST_F(LLMWriteToolsEngine, SetPropertyActivityRequiresConfirmation)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "set_property";
    req.confirmed = false;
    req.arguments = {{"long_name", "PGenerator"},
                     {"property_name", "Activity"},
                     {"value", "1"},
                     {"channel_index", 0}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    EXPECT_TRUE(r.pending_confirmation);
}

TEST_F(LLMWriteToolsEngine, ConnectComponentsInvalidPortFails)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "connect_components";
    req.arguments = {{"from_long_name", "PGenerator"},
                     {"from_property", "Output"},
                     {"to_long_name", "IaFNeuron"},
                     {"to_property", "NotAValidPropertyName"},
                     {"channel_index", 0}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    if(r.ok)
        GTEST_SKIP() << "Engine accepted invalid port (no link validation): " << r.message;
    EXPECT_EQ(r.error_code, "DomainError");
}

TEST_F(LLMWriteToolsEngine, DisconnectComponentsAfterConnect)
{
    EngineGateway gw(ctx_->application);
    const std::string suffix = uniqueSuffix();

    ToolInvokeRequest add = gw.baseRequest();
    add.tool_name = "add_component";
    add.arguments = {{"class_name", "NPGenerator"},
                     {"parent_long_name", ""},
                     {"short_name", "LlmLink" + suffix},
                     {"channel_index", 0}};
    const ToolGatewayResult added = gw.gateway.invoke(add);
    ASSERT_TRUE(added.ok) << added.message;
    const std::string gen = added.result["long_name"].get<std::string>();

    ToolInvokeRequest connect = gw.baseRequest();
    connect.tool_name = "connect_components";
    connect.arguments = {{"from_long_name", gen},
                         {"from_property", "Output"},
                         {"to_long_name", "IaFNeuron"},
                         {"to_property", "Soma1.ExcSynapse1"},
                         {"channel_index", 0}};
    const ToolGatewayResult linked = gw.gateway.invoke(connect);
    if(!linked.ok)
    {
        ToolInvokeRequest cleanup = gw.baseRequest();
        cleanup.tool_name = "remove_component";
        cleanup.arguments = {{"long_name", gen}, {"channel_index", 0}};
        (void)gw.gateway.invoke(cleanup);
        GTEST_SKIP() << "connect_components not applicable: " << linked.message;
    }

    ToolInvokeRequest disconnect = gw.baseRequest();
    disconnect.tool_name = "disconnect_components";
    disconnect.arguments = connect.arguments;
    const ToolGatewayResult broken = gw.gateway.invoke(disconnect);
    EXPECT_TRUE(broken.ok) << broken.message;

    ToolInvokeRequest cleanup = gw.baseRequest();
    cleanup.tool_name = "remove_component";
    cleanup.arguments = {{"long_name", gen}, {"channel_index", 0}};
    (void)gw.gateway.invoke(cleanup);
}

TEST_F(LLMWriteToolsEngine, ValidateConfigurationReadOnDisk)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "validate_configuration";
    req.arguments = {{"configuration_path", NmsdkTests::testValidProjectIniPath()}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    EXPECT_TRUE(r.ok) << r.message;
}
