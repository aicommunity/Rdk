#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#ifndef RDK_LLM_FREE_DIALOGUE_CORPUS_DIR
#define RDK_LLM_FREE_DIALOGUE_CORPUS_DIR "Rdk/Tests/Fixtures/LLM/free-dialogue-corpus"
#endif

namespace fs = std::filesystem;

TEST(FreeDialogueCorpus, LoadsAtLeast30Scenarios)
{
    const fs::path root(RDK_LLM_FREE_DIALOGUE_CORPUS_DIR);
    ASSERT_TRUE(fs::is_directory(root)) << root;
    int count = 0;
    for(const auto& entry : fs::directory_iterator(root))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        EXPECT_TRUE(j.contains("id"));
        EXPECT_TRUE(j.contains("user_text"));
        ++count;
    }
    EXPECT_GE(count, 30);
}

TEST(FreeDialogueCorpus, ExpectsNoHardTaskPathFailFlag)
{
    const fs::path root(RDK_LLM_FREE_DIALOGUE_CORPUS_DIR);
    for(const auto& entry : fs::directory_iterator(root))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        if(j.contains("expect") && j["expect"].is_object())
            EXPECT_TRUE(j["expect"].value("no_hard_task_path_fail", false));
    }
}
