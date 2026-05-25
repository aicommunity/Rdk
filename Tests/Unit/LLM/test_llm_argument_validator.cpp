#include <gtest/gtest.h>

#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(LLMArgumentValidator, RequiresFields)
{
    ULLMToolArgumentValidator v;
    nlohmann::json schema = {{"type", "object"},
                             {"required", nlohmann::json::array({"query"})},
                             {"properties", {{"query", {{"type", "string"}}}}},
                             {"additionalProperties", false}};
    nlohmann::json args = {{"query", "net"}};
    std::string err;
    EXPECT_TRUE(v.validate(args, schema, err));
    nlohmann::json bad = nlohmann::json::object();
    EXPECT_FALSE(v.validate(bad, schema, err));
}
