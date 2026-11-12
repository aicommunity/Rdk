#include <gtest/gtest.h>

#include <cstdlib>

#include "Settings/ULLMResponseLanguage.h"

using namespace RDK::LLM;

TEST(ResponseLanguage, ResolvesExplicitCode)
{
    EXPECT_EQ(resolveResponseLanguage("ru", "en"), "ru");
    EXPECT_EQ(resolveResponseLanguage("en", "ru"), "en");
}

TEST(ResponseLanguage, EmptyConfiguredUsesFallbackWithoutEnv)
{
    const char* old_lang = std::getenv("LANG");
    const char* old_lc = std::getenv("LC_ALL");
#if defined(_WIN32)
    _putenv_s("LANG", "");
    _putenv_s("LC_ALL", "");
#else
    unsetenv("LANG");
    unsetenv("LC_ALL");
#endif
    EXPECT_EQ(resolveResponseLanguage("", "en"), "en");
    if(old_lang)
        setenv("LANG", old_lang, 1);
    if(old_lc)
        setenv("LC_ALL", old_lc, 1);
}

TEST(ResponseLanguage, CyrillicUserTextSelectsRuWhenAuto)
{
    const char* old_lang = std::getenv("LANG");
    const char* old_lc = std::getenv("LC_ALL");
#if defined(_WIN32)
    _putenv_s("LANG", "");
    _putenv_s("LC_ALL", "");
#else
    unsetenv("LANG");
    unsetenv("LC_ALL");
#endif
    EXPECT_TRUE(userTextLooksCyrillic("Расскажи о проекте"));
    EXPECT_FALSE(userTextLooksCyrillic("Tell me about the project"));
    EXPECT_EQ(resolveResponseLanguage("", "Расскажи о проекте и запиши description", "en"), "ru");
    EXPECT_EQ(resolveResponseLanguage("auto", "запиши описание", "en"), "ru");
    EXPECT_EQ(resolveResponseLanguage("en", "Расскажи о проекте", "ru"), "en");
    if(old_lang)
        setenv("LANG", old_lang, 1);
    else
        unsetenv("LANG");
    if(old_lc)
        setenv("LC_ALL", old_lc, 1);
}

TEST(ResponseLanguage, SupportedCodes)
{
    EXPECT_TRUE(isSupportedResponseLanguageCode("en"));
    EXPECT_TRUE(isSupportedResponseLanguageCode("ru"));
    EXPECT_FALSE(isSupportedResponseLanguageCode("xx"));
}

TEST(ResponseLanguage, DisplayName)
{
    EXPECT_EQ(responseLanguageDisplayName("ru"), "Russian");
    EXPECT_EQ(responseLanguageDisplayName("en"), "English");
}

TEST(ResponseLanguage, LanguageFromEnvironmentRu)
{
    const char* old_lang = std::getenv("LANG");
    const char* old_lc = std::getenv("LC_ALL");
#if defined(_WIN32)
    _putenv_s("LANG", "ru_RU.UTF-8");
    _putenv_s("LC_ALL", "");
#else
    setenv("LANG", "ru_RU.UTF-8", 1);
    unsetenv("LC_ALL");
#endif
    EXPECT_EQ(languageFromEnvironment(), "ru");
    EXPECT_EQ(resolveResponseLanguage("", "en"), "ru");
    if(old_lang)
        setenv("LANG", old_lang, 1);
    else
        unsetenv("LANG");
    if(old_lc)
        setenv("LC_ALL", old_lc, 1);
}
