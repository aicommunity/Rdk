#include <gtest/gtest.h>

#include <cstdlib>

#include "Orchestrator/ULLMInputUnderstanding.h"
#include "Orchestrator/ULLMIntentParser.h"
#include "Providers/ULLMMockProvider.h"

using namespace RDK::LLM;

TEST(InputUnderstanding, ParsesLlmJsonIntent)
{
    unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");

    ULLMIntentParser parser;
    const IntentParseResult heuristic = parser.parseDetailed("explain how UNet works");

    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = R"({"intent":"explain","confidence":0.9,"needs_clarification":false})";
    provider.enqueue(mock);

    setenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM", "1", 1);
    const InputUnderstandingResult out =
        understandUserInput(&provider, "explain how UNet works", heuristic);
    EXPECT_EQ(out.intent, LLMIntentKind::Explain);
    EXPECT_GE(out.confidence, 0.85f);
    EXPECT_EQ(out.method, "llm_json");
    unsetenv("NMSDK_LLM_INPUT_ENSEMBLE_LLM");
}
