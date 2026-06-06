#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "Context/UDocSearchIndex.h"

namespace fs = std::filesystem;

TEST(LLMDocSearchIndex, TfIdfRanksMatchingDocHigher)
{
    const fs::path tmp = fs::temp_directory_path() / "rdk_llm_tfidf_test";
    fs::create_directories(tmp);
    {
        std::ofstream(tmp / "arduino.md") << "Arduino Firmata transport protocol\n";
        std::ofstream(tmp / "other.md") << "unrelated database configuration\n";
    }

    RDK::LLM::UDocSearchIndex index;
    index.build({tmp}, 10);
    const auto hits = index.search("arduino firmata", 2);
    ASSERT_GE(hits.size(), 1u);
    EXPECT_NE(hits.front().path.find("arduino"), std::string::npos);

    fs::remove_all(tmp);
}

TEST(LLMDocSearchIndex, SemanticBoostFindsSynonymDoc)
{
    const fs::path tmp = fs::temp_directory_path() / "rdk_llm_semantic_test";
    fs::create_directories(tmp);
    {
        std::ofstream(tmp / "spike.md")
            << "Spiking neural network neuron membrane dynamics\n";
        std::ofstream(tmp / "sql.md") << "SQL database migration scripts\n";
    }

    RDK::LLM::UDocSearchIndex index;
    index.build({tmp}, 10);
    const auto hits = index.search("pulse neuron membrane", 2);
    ASSERT_GE(hits.size(), 1u);
    EXPECT_NE(hits.front().path.find("spike"), std::string::npos);

    fs::remove_all(tmp);
}
