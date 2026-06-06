#include <gtest/gtest.h>

#include "Context/ULinkPatternCatalog.h"
#include "Context/ULLMConnectSemanticsCatalog.h"

using namespace RDK::LLM;

static std::filesystem::path findRepoRoot()
{
    std::filesystem::path p = std::filesystem::current_path();
    for(int i = 0; i < 10 && p.has_parent_path(); ++i)
    {
        if(std::filesystem::exists(p / "CMakeLists.txt"))
            return p;
        p = p.parent_path();
    }
    return std::filesystem::current_path();
}

TEST(LLMLinkPatternCatalog, ParsesTestValidationFixture)
{
    const auto items = buildLinkPatternsFromConfigs(
        findRepoRoot() / "Bin/Configs/TestValidation/test_valid");
    ASSERT_FALSE(items.empty());

    bool found = false;
    for(const auto& it : items)
    {
        if(it.from_port == "Output" && it.to_port == "Soma1.ExcSynapse1")
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST(LLMLinkPatternCatalog, BuildSemanticsFromTestValidation)
{
    const auto entries = buildConnectSemanticsFromConfigs(
        findRepoRoot() / "Bin/Configs/TestValidation/test_valid");
    ASSERT_FALSE(entries.empty());
    bool has_neuron = false;
    for(const auto& e : entries)
    {
        if(e.from_class.find("Neuron") != std::string::npos
           && e.to_class.find("Neuron") != std::string::npos)
        {
            has_neuron = true;
            break;
        }
    }
    EXPECT_TRUE(has_neuron);
}

