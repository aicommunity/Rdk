#include <gtest/gtest.h>

#include "Context/ULLMConnectSemanticsCatalog.h"
#include "Domain/ULLMConnectPortInference.h"
#include "Domain/URdkDomainAccess.h"
#include "Context/ULinkPatternCatalog.h"

#include <filesystem>

using namespace RDK::LLM;

namespace {

ConnectSemanticEntry neuronEntry()
{
    ConnectSemanticEntry e;
    e.from_class = "NSPNeuron";
    e.to_class = "NSPNeuron";
    e.from_port = "LTZone";
    e.to_port = "Soma1.ExcSynapse1";
    e.confidence = 0.95;
    e.count = 10;
    return e;
}

} // namespace

TEST(LLMConnectPortInference, PreferInternalSemanticsOverGeneric)
{
    URdkDomainAccess domain(nullptr);
    ULinkPatternCatalog catalog;

    ULLMConnectSemanticsCatalog semantics;
    const std::filesystem::path tmp = std::filesystem::temp_directory_path()
                                      / "llm_port_inference_semantics.json";
    writeConnectSemantics(tmp, tmp.parent_path() / "manifest.json", {neuronEntry()}, "test", 0);
    ASSERT_TRUE(semantics.loadFromFile(tmp));

    nlohmann::json args = {{"from_long_name", "/ch0/MModel.A"},
                           {"to_long_name", "/ch0/MModel.B"},
                           {"from_property", "Output"},
                           {"to_property", "Input"},
                           {"channel_index", 0}};

    ConnectPortInferenceResult inf = inferConnectPorts(
        args, domain, catalog, 0, &semantics, "connect LTZone to Soma1.ExcSynapse1", true);
    EXPECT_TRUE(inf.ok);
    EXPECT_EQ(args.value("from_property", ""), "LTZone");
    EXPECT_EQ(args.value("to_property", ""), "Soma1.ExcSynapse1");
}
