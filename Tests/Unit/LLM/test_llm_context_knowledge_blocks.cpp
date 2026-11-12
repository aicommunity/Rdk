#include <gtest/gtest.h>

#include "Context/ULLMConnectSemanticsCatalog.h"
#include "Context/ULinkPatternCatalog.h"
#include "Orchestrator/ULLMContextKnowledgeBlocks.h"

#include <filesystem>

using namespace RDK::LLM;

TEST(LLMContextKnowledgeBlocks, LinkPatternHintFromFixtureCatalog)
{
    ULinkPatternCatalog catalog;
    LinkPatternItem item;
    item.from_class = "NSPNeuron";
    item.to_class = "NSPNeuron";
    item.from_port = "LTZone";
    item.to_port = "Soma1.ExcSynapse1";
    item.count = 5;
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_kb_link_patterns.json";
    writeLinkPatterns(tmp, tmp.parent_path() / "manifest.json", {item}, "test", 0);
    ASSERT_TRUE(catalog.loadFromFile(tmp));

    const std::string block =
        buildLinkPatternHintBlock(catalog, "NSPNeuron", "NSPNeuron", 3);
    EXPECT_NE(block.find("Link patterns"), std::string::npos);
    EXPECT_NE(block.find("LTZone"), std::string::npos);
}

TEST(LLMContextKnowledgeBlocks, ConnectSemanticsHintFromCatalog)
{
    ULLMConnectSemanticsCatalog semantics;
    const ConnectSemanticEntry e = [] {
        ConnectSemanticEntry x;
        x.from_class = "A";
        x.to_class = "B";
        x.from_port = "Out";
        x.to_port = "In";
        x.count = 3;
        return x;
    }();
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_kb_connect_sem.json";
    writeConnectSemantics(tmp, tmp.parent_path() / "manifest.json", {e}, "test", 0);
    ASSERT_TRUE(semantics.loadFromFile(tmp));

    const std::string block = buildConnectSemanticsHintBlock(semantics, "A", "B", 2);
    EXPECT_NE(block.find("Connect semantics"), std::string::npos);
    EXPECT_NE(block.find("Out"), std::string::npos);
}
