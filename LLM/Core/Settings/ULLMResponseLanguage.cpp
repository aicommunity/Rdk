#include "ULLMResponseLanguage.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

bool isAlphaLower(char c)
{
    return c >= 'a' && c <= 'z';
}

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

} // namespace

bool isSupportedResponseLanguageCode(const std::string& code)
{
    return code == "en" || code == "ru" || code == "de" || code == "fr" || code == "zh";
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
    if(!configured.empty() && isSupportedResponseLanguageCode(configured))
        return configured;

    const std::string from_env = languageFromEnvironment();
    if(isSupportedResponseLanguageCode(from_env))
        return from_env;

    return system_fallback;
}

} // namespace RDK::LLM
