#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "Context/ULLMLongTermMemoryLoader.h"

using namespace RDK::LLM;

TEST(LLMLongTermMemory, LoadsUserPreferencesAndProjectRules)
{
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() / "nmsdk_llm_memory_test";
    std::filesystem::remove_all(root);

    const std::filesystem::path user_dir = root / "LLM" / "memory" / "user_42";
    std::filesystem::create_directories(user_dir);
    {
        std::ofstream prefs(user_dir / "preferences.json");
        prefs << R"({"response_language":"ru","notes":"prefer metric"})";
    }

    const std::string project_path = "/tmp/unique_project_for_hash.ini";
    const std::string block =
        loadLongTermMemoryBlock(root, 42, project_path);
    EXPECT_NE(block.find("Long-term memory"), std::string::npos);
    EXPECT_NE(block.find("response_language"), std::string::npos);

    std::filesystem::remove_all(root);
}

TEST(LLMLongTermMemory, SkipsGuestUser)
{
    const std::string block = loadLongTermMemoryBlock(std::filesystem::path("."), -1, "/p/x.ini");
    EXPECT_TRUE(block.empty());
}
