#include <gtest/gtest.h>

#include <rdk_application.h>

#include "../../../Core/Application/UProject.h"
#include "Domain/URdkApplicationCommands.h"

using namespace RDK::LLM;

TEST(LLMApplicationCommands, BuildProjectConfigDefaults)
{
    std::string err;
    const nlohmann::json args = {{"project_name", "TestCfg"},
                                 {"channels", nlohmann::json::array({{{"class_name", "Model"}}})}};
    RDK::TProjectConfig cfg = URdkApplicationCommands::buildProjectConfigFromJson(args, nullptr, err);
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(cfg.ProjectName, "TestCfg");
    EXPECT_EQ(cfg.NumChannels, 1);
    EXPECT_EQ(cfg.ChannelsConfig[0].ClassName, "Model");
    EXPECT_EQ(cfg.ChannelsConfig[0].ModelMode, 3);
}

TEST(LLMApplicationCommands, ResolveConfigurationPathAliases)
{
    std::string err;
    nlohmann::json args = {{"configuration_path", "/tmp/myproj"}};
    std::string path = URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
    EXPECT_TRUE(err.empty());
    EXPECT_EQ(path, "/tmp/myproj/project.ini");
}

TEST(LLMApplicationCommands, ResolveProjectIniAutocreateOnly)
{
    std::string err;
    const nlohmann::json args = {{"autocreate_subdirectory", true}};
    const std::string ini = URdkApplicationCommands::resolveProjectIniPath(args, nullptr, err);
    EXPECT_TRUE(err.empty());
    EXPECT_NE(ini.find("Autocreate"), std::string::npos);
    EXPECT_NE(ini.find("project.ini"), std::string::npos);
}

TEST(LLMApplicationCommands, EnsureProjectIniAppends)
{
    EXPECT_EQ(URdkApplicationCommands::ensureProjectIniPath("/data/cfg"), "/data/cfg/project.ini");
    EXPECT_EQ(URdkApplicationCommands::ensureProjectIniPath("/data/cfg/project.ini"),
              "/data/cfg/project.ini");
}
