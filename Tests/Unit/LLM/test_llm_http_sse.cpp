#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

TEST(LLMHttpSse, ParsesOpenAiDeltaContent)
{
    const std::string payload = R"({"choices":[{"delta":{"content":"Hi"}}]})";
    const nlohmann::json j = nlohmann::json::parse(payload);
    ASSERT_TRUE(j["choices"][0]["delta"].contains("content"));
    EXPECT_EQ(j["choices"][0]["delta"]["content"].get<std::string>(), "Hi");
}
