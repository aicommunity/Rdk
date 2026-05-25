#include <gtest/gtest.h>

#include "Context/UDocOllamaEmbeddings.h"

TEST(LLMDocOllamaEmbeddings, DisabledByDefault)
{
    EXPECT_FALSE(RDK::LLM::UDocOllamaEmbeddings::enabled());
}
