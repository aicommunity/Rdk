#ifndef RDK_ULLM_RESPONSE_LANGUAGE_H
#define RDK_ULLM_RESPONSE_LANGUAGE_H

#include <string>

namespace RDK::LLM {

bool isSupportedResponseLanguageCode(const std::string& code);

/// Prefer explicit configured code, then environment, then system_fallback.
std::string resolveResponseLanguage(const std::string& configured,
                                    const std::string& system_fallback = "en");

/// When configured is empty/"auto", detect Cyrillic in user_text → "ru" before env/fallback.
std::string resolveResponseLanguage(const std::string& configured, const std::string& user_text,
                                    const std::string& system_fallback);

bool userTextLooksCyrillic(const std::string& user_text);

std::string languageFromEnvironment();

std::string responseLanguageDisplayName(const std::string& code);

} // namespace RDK::LLM

#endif
