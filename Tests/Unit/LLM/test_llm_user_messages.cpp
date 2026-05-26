#include <gtest/gtest.h>

#include "Settings/ULLMUserMessages.h"

using namespace RDK::LLM;

TEST(LLMUserMessages, ReturnsEnglishAndRussianMessages)
{
    EXPECT_EQ(formatUserMessage("confirmation.apply_hint", "en"),
              "Press Apply to confirm.");
    EXPECT_EQ(formatUserMessage("confirmation.apply_hint", "ru"),
              "Нажмите Apply для подтверждения.");
    EXPECT_EQ(formatUserMessage("confirmation.required", "en", {{"tool_name", "create_configuration"}}),
              "Confirmation required for: create_configuration.");
    EXPECT_EQ(formatUserMessage("confirmation.required", "ru", {{"tool_name", "load_configuration"}}),
              "Требуется подтверждение для: load_configuration.");

    EXPECT_EQ(formatUserMessage("args.missing.configuration_path", "en"),
              "Specify configuration path.");
    EXPECT_EQ(formatUserMessage("args.missing.configuration_path", "ru"),
              "Укажите путь к конфигурации.");

    EXPECT_EQ(formatUserMessage("error.max_rounds", "en"), "Stopped: too many steps.");
    EXPECT_EQ(formatUserMessage("error.max_rounds", "ru"),
              "Остановлено: слишком много шагов.");

    EXPECT_EQ(formatUserMessage("error.no_suitable_tool", "en"),
              "Cannot find a suitable action.");
    EXPECT_EQ(formatUserMessage("error.no_suitable_tool", "ru"),
              "Не найдено подходящее действие.");

    EXPECT_EQ(formatUserMessage("error.index_missing", "en"),
              "Knowledge index unavailable.");
    EXPECT_EQ(formatUserMessage("error.index_missing", "ru"),
              "Индекс знаний недоступен.");
}
