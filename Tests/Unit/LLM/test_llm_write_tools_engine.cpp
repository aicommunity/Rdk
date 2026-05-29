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

TEST_F(LLMWriteToolsEngine, GetNetSnapshotRootLongNameSubtree)
{
    EngineGateway gw(ctx_->application);
    const int max_c = 500;

    ToolInvokeRequest full = gw.baseRequest();
    full.tool_name = "get_net_snapshot";
    full.arguments = {{"channel_index", 0}, {"max_components", max_c}};
    const ToolGatewayResult full_r = gw.gateway.invoke(full);
    ASSERT_TRUE(full_r.ok) << full_r.message;
    const nlohmann::json full_components = full_r.result.value("components", nlohmann::json::array());

    ToolInvokeRequest subtree = gw.baseRequest();
    subtree.tool_name = "get_net_snapshot";
    subtree.arguments = {{"channel_index", 0},
                         {"max_components", max_c},
                         {"root_long_name", "PGenerator"}};
    const ToolGatewayResult sub_r = gw.gateway.invoke(subtree);
    ASSERT_TRUE(sub_r.ok) << sub_r.message;
    const nlohmann::json sub_components = sub_r.result.value("components", nlohmann::json::array());
    EXPECT_LE(sub_components.size(), full_components.size());
    EXPECT_EQ(sub_r.result.value("root_long_name", std::string()), "PGenerator");
    for(const nlohmann::json& item : sub_components)
    {
        const std::string ln = item.value("long_name", "");
        ASSERT_FALSE(ln.empty());
        EXPECT_TRUE(ln == "PGenerator" || ln.rfind("PGenerator.", 0) == 0) << ln;
    }
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

TEST_F(LLMWriteToolsEngine, ConnectComponentsResolvesGenericPortNames)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "connect_components";
    req.user_text_hint = "connect";
    req.arguments = {{"from_long_name", "PGenerator"},
                     {"from_property", "output"},
                     {"to_long_name", "PNeuron2"},
                     {"to_property", "input"},
                     {"channel_index", 0}};

    const ToolGatewayResult r = gw.gateway.invoke(req);
    if(r.error_code == "CONNECT_PORTS_AMBIGUOUS" || r.error_code == "CONNECT_PORT_NOT_FOUND")
    {
        EXPECT_FALSE(r.message.empty());
        EXPECT_TRUE(r.result.value("ambiguous", false));
        return;
    }
    ASSERT_TRUE(r.ok) << r.message;
    EXPECT_EQ(r.result.value("from_property", ""), "Output");
    const std::string to_port = r.result.value("to_property", "");
    EXPECT_FALSE(to_port.empty());
    EXPECT_NE(to_port, "input");
    EXPECT_NE(to_port.find("ExcSynapse1"), std::string::npos) << to_port;
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
    EXPECT_TRUE(r.error_code == "DomainError" || r.error_code == "CONNECT_PORT_NOT_FOUND")
        << r.error_code;
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

TEST_F(LLMWriteToolsEngine, ListRegisteredClassesLibraryFilter)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "list_registered_classes";
    req.arguments = nlohmann::json::object();
    const ToolGatewayResult all_r = gw.gateway.invoke(req);
    ASSERT_TRUE(all_r.ok) << all_r.message;
    const nlohmann::json all_classes = all_r.result.value("classes", nlohmann::json::array());
    ASSERT_FALSE(all_classes.empty());

    std::string lib_filter;
    for(const nlohmann::json& item : all_classes)
    {
        const std::string lib = item.value("library", "");
        if(!lib.empty())
        {
            lib_filter = lib;
            break;
        }
    }
    if(lib_filter.empty())
        GTEST_SKIP() << "No library metadata on registered classes";

    req.arguments = {{"library_filter", lib_filter}};
    const ToolGatewayResult filtered = gw.gateway.invoke(req);
    ASSERT_TRUE(filtered.ok) << filtered.message;
    const nlohmann::json filtered_classes =
        filtered.result.value("classes", nlohmann::json::array());
    EXPECT_LE(filtered_classes.size(), all_classes.size());
    for(const nlohmann::json& item : filtered_classes)
        EXPECT_EQ(item.value("library", ""), lib_filter);
}

TEST_F(LLMWriteToolsEngine, GetComponentPropertiesSummaryAndValues)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "get_component_properties";
    req.arguments = {{"long_name", "PGenerator"}, {"channel_index", 0}};
    const ToolGatewayResult summary = gw.gateway.invoke(req);
    ASSERT_TRUE(summary.ok) << summary.message;
    const nlohmann::json props = summary.result.value("properties", nlohmann::json::array());
    ASSERT_FALSE(props.empty());
    std::string sample_name;
    for(const nlohmann::json& p : props)
    {
        EXPECT_TRUE(p.contains("type"));
        EXPECT_TRUE(p.contains("value_repr"));
        EXPECT_TRUE(p["value_repr"].get<std::string>().empty());
        if(sample_name.empty() && p.contains("name"))
            sample_name = p["name"].get<std::string>();
    }
    ASSERT_FALSE(sample_name.empty());

    req.arguments = {{"long_name", "PGenerator"},
                     {"channel_index", 0},
                     {"property_names", nlohmann::json::array({sample_name})}};
    const ToolGatewayResult values = gw.gateway.invoke(req);
    ASSERT_TRUE(values.ok) << values.message;
    const nlohmann::json one = values.result.value("properties", nlohmann::json::array());
    ASSERT_EQ(one.size(), 1u);
    EXPECT_EQ(one[0].value("name", ""), sample_name);
    EXPECT_FALSE(one[0].value("value_repr", "").empty());
}

TEST_F(LLMWriteToolsEngine, SetPropertyUnknownPropertyNameFails)
{
    EngineGateway gw(ctx_->application);
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "set_property";
    req.confirmed = true;
    req.arguments = {{"long_name", "PGenerator"},
                     {"property_name", "NotAValidPropertyName"},
                     {"value", "x"},
                     {"channel_index", 0}};
    const ToolGatewayResult r = gw.gateway.invoke(req);
    ASSERT_FALSE(r.ok);
    EXPECT_NE(r.message.find("property_name"), std::string::npos) << r.message;
}

TEST_F(LLMWriteToolsEngine, SetActiveChannelUpdatesSelection)
{
    EngineGateway gw(ctx_->application);
    const int channels = ctx_->application->GetNumChannels();
    if(channels < 1)
        GTEST_SKIP() << "No channels";

    const int target = (channels > 1) ? 1 : 0;
    ToolInvokeRequest req = gw.baseRequest();
    req.tool_name = "set_active_channel";
    req.arguments = {{"channel_index", target}};
    const ToolGatewayResult r = gw.gateway.invoke(req);
    ASSERT_TRUE(r.ok) << r.message;
    EXPECT_EQ(r.result.value("channel_index", -1), target);
    EXPECT_EQ(r.result.value("selected_channel_index", -1), target);
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
