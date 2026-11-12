#include <gtest/gtest.h>

#include "Context/ULLMConnectSemanticsCatalog.h"

#include <cstdlib>
#include <filesystem>

using namespace RDK::LLM;

namespace {

std::filesystem::path repoRoot()
{
    const char* env = std::getenv("CMAKE_SOURCE_DIR");
    if(env && *env)
        return std::filesystem::path(env);
    return std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path();
}

} // namespace

TEST(LLMConnectSemanticsCatalog, LoadsFixtureJsonIfPresent)
{
    ULLMConnectSemanticsCatalog catalog;
    const std::filesystem::path json = repoRoot() / "Bin/LLM/index/connect-semantics.json";
    if(!std::filesystem::exists(json))
        GTEST_SKIP() << "connect-semantics.json not built; run llm-index-pack";
    ASSERT_TRUE(catalog.loadFromFile(json));
    EXPECT_FALSE(catalog.empty());
}

TEST(LLMConnectSemanticsCatalog, SuggestNeuronPairFromBuiltPatterns)
{
    LinkPatternItem item;
    item.from_class = "NSPNeuron";
    item.to_class = "NSPNeuron";
    item.from_port = "LTZone";
    item.to_port = "Soma1.ExcSynapse1";
    item.count = 10;

    const std::vector<ConnectSemanticEntry> entries =
        buildConnectSemanticsFromLinkPatterns({item});
    ASSERT_FALSE(entries.empty());

    ULLMConnectSemanticsCatalog catalog;
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_connect_semantics_test.json";
    writeConnectSemantics(tmp, tmp.parent_path() / "manifest.json", entries, "test", 0);
    ASSERT_TRUE(catalog.loadFromFile(tmp));

    const auto suggestions = catalog.suggestContainerPair("NSPNeuron", "NSPNeuron", 1);
    ASSERT_FALSE(suggestions.empty());
    EXPECT_NE(suggestions[0].from_port.find("LTZone"), std::string::npos);
}

TEST(LLMConnectSemanticsCatalog, GoalHintRu)
{
    ConnectSemanticEntry e;
    e.from_class = "NSPNeuron";
    e.to_class = "NSPNeuron";
    e.from_port = "LTZone";
    e.to_port = "Soma1.ExcSynapse1";
    e.labels_ru = {"низкопороговая зона"};

    ULLMConnectSemanticsCatalog catalog;
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_connect_semantics_hint_test.json";
    writeConnectSemantics(tmp, tmp.parent_path() / "manifest.json", {e}, "test", 0);
    ASSERT_TRUE(catalog.loadFromFile(tmp));

    std::string from_port;
    std::string to_port;
    EXPECT_TRUE(catalog.matchGoalPortHint("низкопороговая зона", from_port, to_port));
    EXPECT_EQ(from_port, "LTZone");
}
