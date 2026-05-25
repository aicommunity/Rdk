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
