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

TEST(LLMArgumentValidator, EnforcesTypeEnumAndNumericRange)
{
    ULLMToolArgumentValidator v;
    nlohmann::json schema = {
        {"type", "object"},
        {"required", nlohmann::json::array({"mode", "count"})},
        {"properties",
         {{"mode", {{"type", "string"}, {"enum", nlohmann::json::array({"safe", "fast"})}}},
          {"count", {{"type", "integer"}, {"minimum", 1}, {"maximum", 3}}}}},
        {"additionalProperties", false}};

    std::string err;
    EXPECT_TRUE(v.validate({{"mode", "safe"}, {"count", 2}}, schema, err));
    EXPECT_FALSE(v.validate({{"mode", "unsafe"}, {"count", 2}}, schema, err));
    EXPECT_FALSE(v.validate({{"mode", "safe"}, {"count", 5}}, schema, err));
    EXPECT_FALSE(v.validate({{"mode", "safe"}, {"count", "2"}}, schema, err));
}
