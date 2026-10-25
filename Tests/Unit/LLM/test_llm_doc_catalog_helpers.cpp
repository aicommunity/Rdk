#include <gtest/gtest.h>

#include "Context/ULLMDocCatalogHelpers.h"
#include "LlmTypes.h"

#include <nlohmann/json.hpp>

using namespace RDK::LLM;

TEST(LLMDocCatalogHelpers, DetectsRuInventoryQuery)
{
    EXPECT_TRUE(isCatalogInventoryQuery(
        "расскажи какие доступны модели нейронов. дай ссылки на документацию"));
    EXPECT_TRUE(isCatalogInventoryQuery("список классов Pulse"));
    EXPECT_TRUE(isCatalogInventoryQuery("available neuron models"));
    EXPECT_FALSE(isCatalogInventoryQuery("подключи генератор к нейрону"));
}

TEST(LLMDocCatalogHelpers, DetectsDocLinkRequest)
{
    EXPECT_TRUE(userAskedForDocumentationLinks("дай ссылки на документацию"));
    EXPECT_TRUE(userAskedForDocumentationLinks("please give documentation links"));
    EXPECT_FALSE(userAskedForDocumentationLinks("что на схеме"));
}

TEST(LLMDocCatalogHelpers, LibraryDocsPatternTools)
{
    EXPECT_TRUE(isLibraryDocsOrCatalogReadTool("search_pulse_docs"));
    EXPECT_TRUE(isLibraryDocsOrCatalogReadTool("list_pulse_component_classes"));
    EXPECT_TRUE(isLibraryDocsOrCatalogReadTool("search_project_docs"));
    EXPECT_FALSE(isLibraryDocsOrCatalogReadTool("add_component"));
}

TEST(LLMDocCatalogHelpers, AppendsFooterWhenProseLacksLinks)
{
    TurnToolInvocationView inv;
    inv.tool_name = "search_pulse_docs";
    inv.ok = true;
    inv.doc_links.push_back({"CSNM", "nmsdk-doc:Libraries/Nmsdk-PulseLib/Docs/CSNM.md"});
    inv.doc_links.push_back({"iaF", "nmsdk-doc:Libraries/Nmsdk-PulseLib/Docs/iaF.md"});

    std::string prose = "NSynTCNNeuron is a complex recurrent model...";
    ASSERT_TRUE(appendDocLinkFooterFromToolTrace(
        prose, "какие модели нейронов. дай ссылки на документацию", {inv}));
    EXPECT_NE(prose.find("nmsdk-doc:Libraries/Nmsdk-PulseLib/Docs/CSNM.md"), std::string::npos);
    EXPECT_NE(prose.find("[CSNM]("), std::string::npos);
}

TEST(LLMDocCatalogHelpers, SkipsFooterWhenProseAlreadyHasLinks)
{
    TurnToolInvocationView inv;
    inv.ok = true;
    inv.doc_links.push_back({"X", "nmsdk-doc:Docs/X.md"});
    std::string prose = "See [X](nmsdk-doc:Docs/X.md)";
    EXPECT_FALSE(appendDocLinkFooterFromToolTrace(prose, "дай ссылки", {inv}));
}

TEST(LLMDocCatalogHelpers, ExtractLinksFromSnippetsJson)
{
    nlohmann::json result = {{"snippets",
                              nlohmann::json::array(
                                  {{{"title", "A"},
                                    {"path", "Docs/A.md"},
                                    {"doc_uri", "nmsdk-doc:Docs/A.md"}},
                                   {{"title", "B"}, {"path", "Docs/B.md"}}})}};
    std::vector<DocLinkItem> links;
    extractDocLinksFromToolResult(result, links);
    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].uri, "nmsdk-doc:Docs/A.md");
}
