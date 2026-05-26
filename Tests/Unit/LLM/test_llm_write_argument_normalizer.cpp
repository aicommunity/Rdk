#include <gtest/gtest.h>

#include "Domain/ULLMWriteArgumentNormalizer.h"

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
