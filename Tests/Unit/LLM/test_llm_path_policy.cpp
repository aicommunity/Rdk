#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>

#include <rdk_application.h>

#include "Policy/ULLMPathPolicy.h"

using namespace RDK::LLM;

TEST(LLMPathPolicy, RejectsParentTraversal)
{
    std::string err;
    EXPECT_TRUE(ULLMPathPolicy::normalizePath("/tmp/../etc/passwd", err).empty());
    EXPECT_FALSE(err.empty());
}

TEST(LLMPathPolicy, StrictCanBeDisabled)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    EXPECT_FALSE(ULLMPathPolicy::isStrictEnabled());
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
    EXPECT_TRUE(ULLMPathPolicy::isStrictEnabled());
}

TEST(LLMPathPolicy, DeniesEtcPasswdWhenStrictAndNoAppRoots)
{
    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "1", 1);
    std::string err;
    EXPECT_FALSE(ULLMPathPolicy::isAllowed("/etc/passwd/project.ini", nullptr, err));
    EXPECT_FALSE(err.empty());
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");
}

TEST(LLMPathPolicy, AllowsPathUnderTempDirWhenStrictOff)
{
    const std::filesystem::path tmp =
        std::filesystem::temp_directory_path() / "nmsdk_llm_path_policy_test";
    std::error_code ec;
    std::filesystem::create_directories(tmp, ec);

    setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    std::string err;
    const std::string ini = (tmp / "project.ini").string();
    EXPECT_TRUE(ULLMPathPolicy::isAllowed(ini, nullptr, err));
    unsetenv("NMSDK_LLM_PATH_POLICY_STRICT");

    std::filesystem::remove_all(tmp, ec);
}
