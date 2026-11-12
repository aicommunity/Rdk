#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "Orchestrator/ULLMIntentParser.h"

namespace fs = std::filesystem;

namespace {

std::string fixturesDir()
{
#ifdef RDK_LLM_REGRESSION_FIXTURES_DIR
    return RDK_LLM_REGRESSION_FIXTURES_DIR;
#else
    return "Rdk/Tests/Fixtures/LLM/regression";
#endif
}

RDK::LLM::LLMIntentKind intentFromName(const std::string& name)
{
    if(name == "mutate")
        return RDK::LLM::LLMIntentKind::Mutate;
    if(name == "explain")
        return RDK::LLM::LLMIntentKind::Explain;
    if(name == "plan")
        return RDK::LLM::LLMIntentKind::Plan;
    return RDK::LLM::LLMIntentKind::Query;
}

} // namespace

TEST(LLMRegressionFixtures, JsonCasesMatchIntentRules)
{
    const fs::path dir = fixturesDir();
    ASSERT_TRUE(fs::is_directory(dir)) << "Missing regression dir: " << dir;

    RDK::LLM::ULLMIntentParser parser;
    int cases = 0;
    for(const auto& entry : fs::directory_iterator(dir))
    {
        if(entry.path().extension() != ".json")
            continue;
        std::ifstream in(entry.path());
        nlohmann::json j;
        in >> j;
        ASSERT_TRUE(j.contains("phrase")) << entry.path();
        ASSERT_TRUE(j.contains("expect_intent")) << entry.path();
        const std::string phrase = j["phrase"].get<std::string>();
        const auto expected = intentFromName(j["expect_intent"].get<std::string>());
        const RDK::LLM::IntentParseResult r = parser.parseDetailed(phrase);
        EXPECT_EQ(r.kind, expected) << entry.path().filename();
        ++cases;
    }
    EXPECT_GE(cases, 4) << "Add cases under " << dir;
}

TEST(LLMRegressionFixtures, LegacyTxtPhraseStillQuery)
{
    const fs::path file = fs::path(fixturesDir()) / "q_list_components.txt";
    if(!fs::exists(file))
        GTEST_SKIP() << "No legacy txt fixture";
    std::ifstream in(file);
    std::string phrase;
    std::getline(in, phrase);
    const auto r = RDK::LLM::ULLMIntentParser{}.parseDetailed(phrase);
    EXPECT_EQ(r.kind, RDK::LLM::LLMIntentKind::Query);
}
