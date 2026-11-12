#include <gtest/gtest.h>

#include "Domain/ULLMNameResolution.h"

using namespace RDK::LLM;

namespace {

std::vector<std::string> sampleNeuronRegistry()
{
    return {"NSPNeuron",  "NPNeuron",   "NNewSPNeuron", "NSPNeuronBio", "NSPNeuronGen",
            "NSynSPNeuron", "NLPNeuron",  "NSPNeuronBio2"};
}

} // namespace

TEST(LLMWriteArgumentNormalizer, ExtractClassNameQueryPrefersLastTokenOverNeuronSubstring)
{
    EXPECT_EQ(extractClassNameQuery("", "добавь spneuron"), "spneuron");
    EXPECT_EQ(extractClassNameQuery("spneuron", ""), "spneuron");
}

TEST(LLMWriteArgumentNormalizer, ExtractClassNameQueryPrefersUserTokenOverMergedField)
{
    EXPECT_EQ(extractClassNameQuery("NPNeuron", "добавь nspneuron"), "nspneuron");
}

TEST(LLMWriteArgumentNormalizer, ExtractClassNameQueryWholeMessageKeyword)
{
    EXPECT_EQ(extractClassNameQuery("", "neuron"), "neuron");
}

TEST(LLMWriteArgumentNormalizer, ResolveExactCaseSensitive)
{
    const auto r = resolveRegisteredClassName("NSPNeuron", sampleNeuronRegistry());
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(r.class_name, "NSPNeuron");
}

TEST(LLMWriteArgumentNormalizer, ResolveCaseInsensitiveSingle)
{
    const auto r = resolveRegisteredClassName("nspneuron", sampleNeuronRegistry());
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(r.class_name, "NSPNeuron");
}

TEST(LLMWriteArgumentNormalizer, ResolveCaseInsensitiveNotConfusedWithNpneuron)
{
    const auto r = resolveRegisteredClassName("npneuron", sampleNeuronRegistry());
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(r.class_name, "NPNeuron");
}

TEST(LLMWriteArgumentNormalizer, ResolveFuzzyShowsListWhenSeveralClose)
{
    const auto r = resolveRegisteredClassName("spneuron", sampleNeuronRegistry());
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Ambiguous);
    ASSERT_GE(r.candidates.size(), 2u);
}

TEST(LLMWriteArgumentNormalizer, FuzzyPropertyTypoToCanonical)
{
    const std::vector<std::string> catalog = {
        "StructureBuildMode", "NumSomaMembraneParts", "NumDendriteMembranePartsVec", "Output"};
    const auto r =
        resolvePropertyNameFromCatalog("numDendridetMembranePartsVec", catalog);
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(r.class_name, "NumDendriteMembranePartsVec");
}

TEST(LLMWriteArgumentNormalizer, FuzzyPropertyCaseInsensitive)
{
    const std::vector<std::string> catalog = {"NumSomaMembraneParts", "Output"};
    const auto r = resolvePropertyNameFromCatalog("numsomaMembraneParts", catalog);
    ASSERT_EQ(r.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(r.class_name, "NumSomaMembraneParts");
}
