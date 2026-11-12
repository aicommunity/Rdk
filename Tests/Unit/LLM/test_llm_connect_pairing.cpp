#include <gtest/gtest.h>

#include "Context/ULLMConnectSemanticsCatalog.h"
#include "Context/ULinkPatternCatalog.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMConnectPairing.h"

#include <filesystem>

using namespace RDK::LLM;

namespace {

nlohmann::json neuronComponents()
{
    return nlohmann::json::array({{{"long_name", "/ch0/A"},
                                  {"short_name", "A"},
                                  {"class_name", "NSPNeuron"}},
                                 {{"long_name", "/ch0/B"},
                                  {"short_name", "B"},
                                  {"class_name", "NSPNeuron"}},
                                 {{"long_name", "/ch0/C"},
                                  {"short_name", "C"},
                                  {"class_name", "NSPNeuron"}}});
}

ULLMConnectSemanticsCatalog loadNeuronSemantics()
{
    ULLMConnectSemanticsCatalog catalog;
    ConnectSemanticEntry e;
    e.from_class = "NSPNeuron";
    e.to_class = "NSPNeuron";
    e.from_port = "LTZone";
    e.to_port = "Soma1.ExcSynapse1";
    e.confidence = 0.95;
    e.count = 10;
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_pairing_semantics.json";
    writeConnectSemantics(tmp, tmp.parent_path() / "manifest.json", {e}, "test", 0);
    catalog.loadFromFile(tmp);
    return catalog;
}

} // namespace

TEST(LLMConnectPairing, RequiresDomain)
{
    PairingRequest req;
    req.endpoints = {"/ch0/A", "/ch0/B"};
    const PairingResult result = buildPairingCandidates(req);
    ASSERT_FALSE(result.issues.empty());
    EXPECT_EQ(result.issues[0], "no_pairs");
}

TEST(LLMConnectPairing, ChainThreeNodes)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;
    static ULLMConnectSemanticsCatalog semantics = loadNeuronSemantics();

    const nlohmann::json components = neuronComponents();
    PairingRequest req;
    req.endpoints = {"/ch0/A", "/ch0/B", "/ch0/C"};
    req.snapshot_components = &components;
    req.domain = &domain;
    req.catalog = &catalog;
    req.semantics = &semantics;
    req.topology = ConnectTopology::Chain;

    const PairingResult result = buildPairingCandidates(req);
    ASSERT_TRUE(result.issues.empty());
    ASSERT_EQ(result.pairs.size(), 2u);
    EXPECT_EQ(result.pairs[0].from_long_name, "/ch0/A");
    EXPECT_EQ(result.pairs[0].to_long_name, "/ch0/B");
    EXPECT_EQ(result.pairs[1].from_long_name, "/ch0/B");
    EXPECT_EQ(result.pairs[1].to_long_name, "/ch0/C");
}

TEST(LLMConnectPairing, TreeExplicitHub)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;

    PairingRequest req;
    req.endpoints = {"/ch0/A", "/ch0/B", "/ch0/C"};
    req.hub_long_name = "/ch0/A";
    req.domain = &domain;
    req.catalog = &catalog;
    req.topology = ConnectTopology::Tree;

    const PairingResult result = buildPairingCandidates(req);
    ASSERT_TRUE(result.issues.empty());
    ASSERT_EQ(result.pairs.size(), 2u);
    EXPECT_EQ(result.pairs[0].from_long_name, "/ch0/A");
    EXPECT_EQ(result.pairs[0].to_long_name, "/ch0/B");
    EXPECT_EQ(result.pairs[1].to_long_name, "/ch0/C");
}

TEST(LLMConnectPairing, TreeAutoHubFallbackToFirstEndpoint)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;

    PairingRequest req;
    req.endpoints = {"/ch0/A", "/ch0/X", "/ch0/Y"};
    req.domain = &domain;
    req.catalog = &catalog;
    req.topology = ConnectTopology::Tree;

    const PairingResult result = buildPairingCandidates(req);
    ASSERT_TRUE(result.issues.empty());
    ASSERT_EQ(result.pairs.size(), 2u);
    EXPECT_EQ(result.pairs[0].from_long_name, "/ch0/A");
}
