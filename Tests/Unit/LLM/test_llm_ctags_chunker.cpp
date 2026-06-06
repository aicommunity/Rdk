#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>

#include "Knowledge/UDocCtagsChunker.h"

namespace fs = std::filesystem;

namespace {

fs::path makeTempCpp()
{
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return fs::temp_directory_path() / ("nmsdk_llm_ctags_" + std::to_string(stamp) + ".cpp");
}

} // namespace

TEST(LLMCtagsChunker, ExtractsFunctionLevelChunks)
{
    const fs::path tmp = makeTempCpp();
    {
        std::ofstream out(tmp);
        out << "int alphaHelper(int x)\n";
        out << "{\n";
        out << "    return x + 1;\n";
        out << "}\n\n";
        out << "static double betaWorker(double a, double b)\n";
        out << "{\n";
        out << "    const double c = a + b;\n";
        out << "    return c * 0.5;\n";
        out << "}\n";
    }

    const auto chunks = RDK::LLM::UDocCtagsChunker::chunkFile(tmp, 80);
    fs::remove(tmp);

    ASSERT_GE(chunks.size(), 2u);
    EXPECT_NE(chunks[0].title.find("alphaHelper"), std::string::npos);
    EXPECT_NE(chunks[1].title.find("betaWorker"), std::string::npos);
    EXPECT_EQ(chunks[0].start_line, 1);
    EXPECT_GT(chunks[1].start_line, chunks[0].start_line);
    EXPECT_NE(chunks[1].excerpt.find("return c * 0.5"), std::string::npos);
}
