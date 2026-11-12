#include <gtest/gtest.h>

#include "Domain/ULLMEntityPathCanonicalizer.h"

using namespace RDK::LLM;

TEST(LLMEntityPathCanonicalizer, ReadsEntityFieldsFromSchema)
{
    const nlohmann::json schema = {
        {"type", "object"},
        {"properties",
         {{"long_name", {{"type", "string"}, {"x-llm-semantic", "entity_long_name"}}},
          {"class_name", {{"type", "string"}, {"x-llm-semantic", "registered_class"}}}}}};
    const std::vector<std::string> fields = entityPathFieldsFromSchema(schema);
    ASSERT_EQ(fields.size(), 1u);
    EXPECT_EQ(fields.front(), "long_name");
}
