#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "TrustBoundary/ULLMTrustBoundary.h"

namespace fs = std::filesystem;

namespace {

std::string fixturesDir()
{
#ifdef RDK_LLM_ADVERSARIAL_FIXTURES_DIR
    return RDK_LLM_ADVERSARIAL_FIXTURES_DIR;
#else
    return "Rdk/Tests/Fixtures/LLM/adversarial";
#endif
}

} // namespace

TEST(LLMAdversarialFixtures, SanitizerNeutralizesInjectionPayloads)
{
    const fs::path dir = fixturesDir();
    ASSERT_TRUE(fs::is_directory(dir)) << "Missing adversarial dir: " << dir;

    int cases = 0;
    for(const auto& entry : fs::directory_iterator(dir))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        ASSERT_TRUE(j.contains("input")) << entry.path();
        const std::string input = j["input"].get<std::string>();
        const std::string out = RDK::LLM::sanitizeUntrustedToolContent(input);

        if(j.contains("must_not_contain"))
        {
            for(const auto& forbidden : j["must_not_contain"])
            {
                const std::string needle = forbidden.get<std::string>();
                EXPECT_EQ(out.find(needle), std::string::npos)
                    << entry.path().filename() << " still contains: " << needle;
            }
        }
        if(j.contains("expect_contains"))
        {
            for(const auto& expected : j["expect_contains"])
            {
                const std::string token = expected.get<std::string>();
                EXPECT_NE(out.find(token), std::string::npos)
                    << entry.path().filename() << " missing: " << token;
            }
        }
        ++cases;
    }
    EXPECT_GE(cases, 2) << "Add JSON fixtures under " << dir;
}

TEST(LLMAdversarialFixtures, TruncatesOversizedPayload)
{
    const std::string huge(9000, 'x');
    const std::string out = RDK::LLM::sanitizeUntrustedToolContent(huge);
    EXPECT_LT(out.size(), huge.size());
    EXPECT_NE(out.find("...[truncated]"), std::string::npos);
}
