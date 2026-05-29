#include <gtest/gtest.h>

#include "Domain/ULLMNameResolution.h"

using namespace RDK::LLM;

namespace {

nlohmann::json sampleComponents()
{
    return nlohmann::json::array({
        {{"long_name", "MModel.Neuron1"}, {"short_name", "Neuron1"}, {"class_name", "NSPNeuron"}},
        {{"long_name", "MModel.Neuron2"}, {"short_name", "Neuron2"}, {"class_name", "NPNeuron"}},
    });
}

} // namespace

TEST(LLMNameResolution, PickFromNumberedListClassName)
{
    const nlohmann::json candidates = {
        {{"class_name", "NSPNeuron"}, {"score", 0.9}},
        {{"class_name", "NPNeuron"}, {"score", 0.8}},
    };
    ASSERT_TRUE(pickFromNumberedList("1", candidates, "class_name").has_value());
    EXPECT_EQ(*pickFromNumberedList("1", candidates, "class_name"), "NSPNeuron");
    EXPECT_FALSE(pickFromNumberedList("9", candidates, "class_name").has_value());
}

TEST(LLMNameResolution, PickFromNumberedListLongName)
{
    const nlohmann::json candidates = {
        {{"long_name", "MModel.Neuron1"}, {"short_name", "Neuron1"}, {"score", 0.98}},
    };
    EXPECT_EQ(*pickFromNumberedList("1", candidates, "long_name"), "MModel.Neuron1");
    EXPECT_EQ(*pickFromNumberedList("neuron1", candidates, "long_name"), "MModel.Neuron1");
}

TEST(LLMNameResolution, ResolveComponentExactCase)
{
    const auto r = resolveComponentEntity("Neuron1", sampleComponents());
    ASSERT_EQ(r.status, ComponentEntityResolution::Status::Resolved);
    EXPECT_EQ(r.canonical_long_name, "MModel.Neuron1");
}

TEST(LLMNameResolution, ResolveComponentCaseInsensitive)
{
    const auto r = resolveComponentEntity("neuron1", sampleComponents());
    ASSERT_EQ(r.status, ComponentEntityResolution::Status::Resolved);
    EXPECT_EQ(r.canonical_long_name, "MModel.Neuron1");
}

TEST(LLMNameResolution, ResolveComponentAmbiguousSubstring)
{
    const auto r = resolveComponentEntity("neuron", sampleComponents());
    ASSERT_EQ(r.status, ComponentEntityResolution::Status::Ambiguous);
    EXPECT_GE(r.candidates.size(), 2u);
}

TEST(LLMNameResolution, ResolvesExplicitClassFromRussianUserText)
{
    const std::vector<std::string> registered = {"NLPNeuron", "NPNeuron", "NSPNeuron"};
    const std::string token = extractClassNameTokenFromUserText("добавь три нейрона NLPNeuron");
    EXPECT_EQ(token, "NLPNeuron");
    const RegisteredClassResolution resolved =
        resolveRegisteredClassName(token, registered);
    ASSERT_EQ(resolved.status, RegisteredClassResolution::Status::Resolved);
    EXPECT_EQ(resolved.class_name, "NLPNeuron");
}
