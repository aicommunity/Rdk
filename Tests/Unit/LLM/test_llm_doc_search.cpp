#include <gtest/gtest.h>

#include "Context/UDocSearchHelper.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

TEST(LlmDocSearch, RanksByTokenOverlap)
{
    const fs::path tmp = fs::temp_directory_path() / "rdk_llm_doc_search_test";
    fs::create_directories(tmp);
    std::ofstream(tmp / "alpha.md") << "# Alpha\narduino firmata transport\n";
    std::ofstream(tmp / "beta.md") << "# Beta\nunrelated topic only\n";

    const auto hits = RDK::LLM::UDocSearchHelper::searchRoots({tmp}, "arduino firmata", 3);
    ASSERT_GE(hits.size(), 1u);
    EXPECT_GT(hits.front().score, 0.5);
    EXPECT_NE(hits.front().path.find("alpha"), std::string::npos);

    fs::remove_all(tmp);
}

TEST(LlmDocSearch, EmptyQueryReturnsEmpty)
{
    const auto hits = RDK::LLM::UDocSearchHelper::searchRoots({}, "  ", 5);
    EXPECT_TRUE(hits.empty());
}
