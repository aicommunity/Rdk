#include <gtest/gtest.h>

#include "Orchestrator/ULLMQueryNormalizer.h"
#include "Providers/ULLMMockProvider.h"

using namespace RDK::LLM;

TEST(LLMQueryNormalizer, EnglishPassThrough)
{
    ULLMMockProvider provider;
    const QueryNormalizeResult out =
        normalizeUserQueryForPlanning(provider, "add two NSPNeuron and connect", true);
    EXPECT_TRUE(out.ok);
    EXPECT_EQ(out.detected_lang, "en");
    EXPECT_EQ(out.text_en, "add two NSPNeuron and connect");
    EXPECT_FALSE(out.used_llm_translate);
}

TEST(LLMQueryNormalizer, RussianUsesLlmTranslation)
{
    ULLMMockProvider provider;
    LLMCompletionResult tr;
    tr.ok = true;
    tr.text = "add two NSPNeuron and connect them";
    provider.enqueue(tr);

    const QueryNormalizeResult out =
        normalizeUserQueryForPlanning(provider, "добавь два NSPNeuron и соедини их", true);
    EXPECT_TRUE(out.ok);
    EXPECT_EQ(out.detected_lang, "ru");
    EXPECT_EQ(out.text_en, "add two NSPNeuron and connect them");
    EXPECT_TRUE(out.used_llm_translate);
}

TEST(LLMQueryNormalizer, TranslationFailureFallsBackToOriginal)
{
    ULLMMockProvider provider;
    LLMCompletionResult tr;
    tr.ok = false;
    tr.error_message = "mock failure";
    provider.enqueue(tr);

    const std::string original = "добавь два нейрона";
    const QueryNormalizeResult out = normalizeUserQueryForPlanning(provider, original, true);
    EXPECT_FALSE(out.ok);
    EXPECT_EQ(out.detected_lang, "ru");
    EXPECT_EQ(out.text_en, original);
    EXPECT_FALSE(out.used_llm_translate);
}
