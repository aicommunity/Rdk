#include <gtest/gtest.h>

#include <fstream>

#include <rdk_application.h>

#include "../../../Core/Application/UProject.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Policy/ULLMPathPolicy.h"
#include "Tools/ApplicationToolAudit.h"
#include "Tools/ApplicationToolHelpers.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Gui/ULLMNoopPresentationSink.h"
#include "Orchestrator/ULLMLifecycleArgumentGate.h"
#include "LlmPublicApi.h"

#include <cstdlib>

using namespace RDK::LLM;

namespace {

std::string fixturePath(const char* name)
{
    return std::string(CMAKE_SOURCE_DIR) + "/Rdk/Tests/Fixtures/LLM/tools/" + name;
}

nlohmann::json loadJsonFixture(const char* name)
{
    std::ifstream in(fixturePath(name));
    EXPECT_TRUE(in.good()) << fixturePath(name);
    nlohmann::json j;
    in >> j;
    return j;
}

} // namespace

TEST(LLMApplicationFixtures, MinimalCreateConfigJsonParses)
{
    const nlohmann::json args = loadJsonFixture("create_configuration_minimal.json");
    std::string err;
    RDK::TProjectConfig cfg = URdkApplicationCommands::buildProjectConfigFromJson(args, nullptr, err);
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(cfg.ProjectName, "MinimalTest");
}

TEST(LLMApplicationFixtures, InvalidLoadPathDeniedByPolicy)
{
    const nlohmann::json args = loadJsonFixture("load_configuration_invalid_path.json");
    std::string err;
    const std::string path = URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
    ASSERT_FALSE(path.empty());

    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "1", 1);
    std::string path_err;
    EXPECT_FALSE(ULLMPathPolicy::isAllowed(path, nullptr, path_err));
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMApplicationFixtures, ValidateConfigurationNoPresentation)
{
    struct CountingSink : ILLMPresentationSink {
        int apply_count = 0;
        void apply(const LLMPresentationEvent& ev) override
        {
            ++apply_count;
            (void)ev;
        }
    } sink;

    ApplicationCommandResult cmd;
    cmd.status = {};
    cmd.presentation = LLMPresentationEffect::None;
    applyPresentationFromCommand(&sink, cmd);
    EXPECT_EQ(sink.apply_count, 0);

    cmd.presentation = LLMPresentationEffect::FullShellRefresh;
    cmd.update_context = true;
    cmd.resolved_configuration_path = "/tmp/x/project.ini";
    applyPresentationFromCommand(&sink, cmd);
    EXPECT_EQ(sink.apply_count, 1);
}

TEST(LLMApplicationFixtures, CoreRegistryIncludesConfigurationTools)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    EXPECT_NE(registry.find("create_configuration"), nullptr);
    EXPECT_NE(registry.find("load_configuration"), nullptr);
    EXPECT_NE(registry.find("validate_configuration"), nullptr);
}

TEST(LLMApplicationFixtures, RecentConfigurationsDedupeAndIndex)
{
    struct RecentSink : ILLMPresentationSink {
        std::vector<std::string> paths;
        explicit RecentSink(std::vector<std::string> p) : paths(std::move(p)) {}
        void apply(const LLMPresentationEvent& event) override { (void)event; }
        std::vector<std::string> recentConfigurationPaths() const override { return paths; }
    };

    RDK::UApplication app;
    const std::list<std::string> history = {"/tmp/ProjA", "/tmp/ProjC/project.ini"};
    ASSERT_TRUE(app.SetLastProjectsList(history));

    const std::vector<std::string> gui_recent = {"/tmp/ProjA/project.ini",
                                                  "/tmp/ProjB"};

    LLMServices& svc = LLMServices::instance();
    if(!svc.isInitialized())
        svc.initialize(&app, nullptr);
    svc.setPresentationSink(std::make_unique<RecentSink>(gui_recent));

    const nlohmann::json list = svc.applicationCommands().listRecentConfigurations();
    const nlohmann::json items = list.value("items", nlohmann::json::array());
    ASSERT_EQ(items.size(), 3);
    EXPECT_EQ(items.at(0).value("path", ""), std::string("/tmp/ProjA/project.ini"));
    EXPECT_EQ(items.at(1).value("path", ""), std::string("/tmp/ProjB/project.ini"));
    EXPECT_EQ(items.at(2).value("path", ""), std::string("/tmp/ProjC/project.ini"));

    const ApplicationCommandResult bad = svc.applicationCommands().openRecentConfiguration(
        99, "close");
    EXPECT_FALSE(bad.status.ok());
}

TEST(LLMApplicationFixtures, LifecycleMergeLastAndIndex)
{
    struct RecentSink : ILLMPresentationSink {
        std::vector<std::string> paths;
        explicit RecentSink(std::vector<std::string> p) : paths(std::move(p)) {}
        void apply(const LLMPresentationEvent& event) override { (void)event; }
        std::vector<std::string> recentConfigurationPaths() const override { return paths; }
    };

    RDK::UApplication app;
    const std::list<std::string> history = {};
    ASSERT_TRUE(app.SetLastProjectsList(history));

    const std::vector<std::string> gui_recent = {"/tmp/Proj1/project.ini",
                                                  "/tmp/Proj2/project.ini"};

    LLMServices& svc = LLMServices::instance();
    if(!svc.isInitialized())
        svc.initialize(&app, nullptr);
    svc.setPresentationSink(std::make_unique<RecentSink>(gui_recent));

    PendingToolArguments pending;
    pending.tool_name = "load_configuration";
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged_last = mergeArgumentsFromUserText(pending, "last", &app);
    ASSERT_TRUE(merged_last.contains("configuration_path"));
    EXPECT_EQ(merged_last.value("configuration_path", ""),
              std::string("/tmp/Proj1/project.ini"));

    const nlohmann::json merged_idx = mergeArgumentsFromUserText(pending, "2", &app);
    ASSERT_TRUE(merged_idx.contains("configuration_path"));
    EXPECT_EQ(merged_idx.value("configuration_path", ""),
              std::string("/tmp/Proj2/project.ini"));
}
