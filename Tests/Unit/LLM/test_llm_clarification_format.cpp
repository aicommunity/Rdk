#include <gtest/gtest.h>

#include "Orchestrator/ULLMClarificationFormat.h"

using namespace RDK::LLM;

TEST(LLMClarificationFormat, ParentFieldShortWithoutList)
{
    nlohmann::json payload = {{"kind", "component"},
                              {"field", "parent_long_name"},
                              {"candidates",
                               nlohmann::json::array({{{"long_name", "PNeuron"},
                                                       {"class_name", "NSPNeuronGen"}},
                                                      {{"long_name", "PGenerator"},
                                                       {"class_name", "NPGenerator"}}})}};

    const std::string short_msg = formatClarificationMessage(payload, false);
    EXPECT_NE(short_msg.find("Where should I add"), std::string::npos);
    EXPECT_EQ(short_msg.find("PNeuron"), std::string::npos);
    EXPECT_EQ(short_msg.find("Multiple components match"), std::string::npos);

    const std::string with_list = formatClarificationMessage(payload, true);
    EXPECT_NE(with_list.find("Where should I add"), std::string::npos);
    EXPECT_NE(with_list.find("PNeuron"), std::string::npos);
    EXPECT_NE(with_list.find("PGenerator"), std::string::npos);
}

TEST(LLMClarificationFormat, ComponentEntityKeepsMatchWording)
{
    nlohmann::json payload = {
        {"kind", "component"},
        {"candidates", nlohmann::json::array({{{"long_name", "A"}, {"class_name", "X"}}})}};

    const std::string short_msg = formatClarificationMessage(payload, false);
    EXPECT_NE(short_msg.find("Multiple components match"), std::string::npos);
    EXPECT_EQ(short_msg.find("\n1."), std::string::npos);

    const std::string with_list = formatClarificationMessage(payload, true);
    EXPECT_NE(with_list.find("1. A"), std::string::npos);
}

TEST(LLMClarificationFormat, ClassShortOmitsNumberedDump)
{
    nlohmann::json payload = {
        {"kind", "class"},
        {"candidates",
         nlohmann::json::array({{{"class_name", "NSPNeuron"}}, {{"class_name", "NPNeuron"}}})}};

    const std::string short_msg = formatClarificationMessage(payload, false);
    EXPECT_NE(short_msg.find("exact component class"), std::string::npos);
    EXPECT_EQ(short_msg.find("NSPNeuron"), std::string::npos);

    const std::string with_list = formatClarificationMessage(payload, true);
    EXPECT_NE(with_list.find("NSPNeuron"), std::string::npos);
}

TEST(LLMClarificationFormat, EnrichPrependsLastAdd)
{
    nlohmann::json payload = {
        {"kind", "class"},
        {"candidates",
         nlohmann::json::array({{{"class_name", "NNeuron"}}, {{"class_name", "NSPNeuron"}}})}};
    enrichClassDisambiguationWithLastAdd(payload, "NSPNeuronGen");
    ASSERT_TRUE(payload["candidates"].is_array());
    ASSERT_GE(payload["candidates"].size(), 1u);
    EXPECT_EQ(payload["candidates"][0].value("class_name", ""), "NSPNeuronGen");
    EXPECT_EQ(payload["candidates"][0].value("label", ""), "same as last");
    EXPECT_EQ(payload.value("last_added_class", ""), "NSPNeuronGen");

    const std::string msg = formatClarificationMessage(payload, true);
    EXPECT_NE(msg.find("Same as last time"), std::string::npos);
    EXPECT_NE(msg.find("NSPNeuronGen"), std::string::npos);
    EXPECT_NE(msg.find("same as last"), std::string::npos);
}

TEST(LLMClarificationFormat, EnrichDoesNotDuplicateLastAdd)
{
    nlohmann::json payload = {
        {"kind", "class"},
        {"candidates", nlohmann::json::array({{{"class_name", "NSPNeuronGen"}}})}};
    enrichClassDisambiguationWithLastAdd(payload, "NSPNeuronGen");
    EXPECT_EQ(payload["candidates"].size(), 1u);
}

TEST(LLMClarificationFormat, NullPayloadDoesNotThrow)
{
    nlohmann::json null_payload;
    EXPECT_NO_THROW({
        const std::string msg = formatClarificationMessage(null_payload, true);
        EXPECT_FALSE(msg.empty());
    });
}

TEST(LLMClarificationFormat, NullCandidateElementsSkipped)
{
    nlohmann::json payload = {{"kind", "class"},
                              {"candidates", nlohmann::json::array({nullptr, {{"class_name", "A"}}})}};
    EXPECT_NO_THROW({
        const std::string msg = formatClarificationMessage(payload, true);
        EXPECT_NE(msg.find("A"), std::string::npos);
    });
}
