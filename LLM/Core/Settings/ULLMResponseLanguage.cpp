#include "ULLMResponseLanguage.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

std::string normalizeEnvLanguage(const char* env)
{
    if(!env || !*env)
        return {};
    std::string s;
    for(const char* p = env; *p && *p != '.' && *p != '_'; ++p)
    {
        const unsigned char uc = static_cast<unsigned char>(*p);
        if(std::isalpha(uc))
            s += static_cast<char>(std::tolower(uc));
        else
            break;
    }
    return s;
}

bool isAutoOrEmptyConfigured(const std::string& configured)
{
    if(configured.empty())
        return true;
    std::string lower = configured;
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lower == "auto";
}

} // namespace

bool isSupportedResponseLanguageCode(const std::string& code)
{
    return code == "en" || code == "ru" || code == "de" || code == "fr" || code == "zh";
}

bool userTextLooksCyrillic(const std::string& user_text)
{
    // UTF-8 Cyrillic letters typically start with 0xD0/0xD1 (U+0400–U+04FF).
    for(std::size_t i = 0; i + 1 < user_text.size(); ++i)
    {
        const unsigned char b0 = static_cast<unsigned char>(user_text[i]);
        const unsigned char b1 = static_cast<unsigned char>(user_text[i + 1]);
        if(b0 == 0xD0 || b0 == 0xD1)
        {
            if((b1 & 0xC0) == 0x80)
                return true;
        }
    }
    return false;
}

std::string languageFromEnvironment()
{
    const char* lang = std::getenv("LC_ALL");
    if(!lang || !*lang)
        lang = std::getenv("LANG");
    if(!lang || !*lang)
        lang = std::getenv("LANGUAGE");
    return normalizeEnvLanguage(lang);
}

std::string responseLanguageDisplayName(const std::string& code)
{
    if(code == "ru")
        return "Russian";
    if(code == "de")
        return "German";
    if(code == "fr")
        return "French";
    if(code == "zh")
        return "Chinese";
    return "English";
}

std::string resolveResponseLanguage(const std::string& configured,
                                    const std::string& system_fallback)
{
    if(!configured.empty() && !isAutoOrEmptyConfigured(configured)
       && isSupportedResponseLanguageCode(configured))
        return configured;

    const std::string from_env = languageFromEnvironment();
    if(isSupportedResponseLanguageCode(from_env))
        return from_env;

    return system_fallback;
}

std::string resolveResponseLanguage(const std::string& configured, const std::string& user_text,
                                    const std::string& system_fallback)
{
    if(!configured.empty() && !isAutoOrEmptyConfigured(configured)
       && isSupportedResponseLanguageCode(configured))
        return configured;

    if(isAutoOrEmptyConfigured(configured) && userTextLooksCyrillic(user_text))
        return "ru";

    const std::string from_env = languageFromEnvironment();
    if(isSupportedResponseLanguageCode(from_env))
        return from_env;

    return system_fallback;
}

} // namespace RDK::LLM
