#include <gtest/gtest.h>

#include "Http/ULLMHttpRetry.h"

TEST(LLMHttpRetry, ParsesRetryAfterSeconds)
{
    EXPECT_EQ(RDK::LLM::parseRetryAfterDelayMs("5"), 5000);
    EXPECT_EQ(RDK::LLM::parseRetryAfterDelayMs("  12 "), 12000);
    EXPECT_EQ(RDK::LLM::parseRetryAfterDelayMs(""), 0);
    EXPECT_EQ(RDK::LLM::parseRetryAfterDelayMs("not-a-number"), 0);
}

TEST(LLMHttpRetry, ProviderDelayUsesMaxOfBackoffAndHeader)
{
    const int delay = RDK::LLM::providerRetryDelayMs(0, "5");
    EXPECT_GE(delay, 5000);
    EXPECT_LE(delay, 60000);
}

TEST(LLMHttpRetry, ProviderDelayCapsAtMax)
{
    const int delay = RDK::LLM::providerRetryDelayMs(0, "9999", 60000);
    EXPECT_EQ(delay, 60000);
}
