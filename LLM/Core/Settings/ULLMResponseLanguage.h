#ifndef RDK_ULLM_RESPONSE_LANGUAGE_H
#define RDK_ULLM_RESPONSE_LANGUAGE_H

#include <string>

namespace RDK::LLM {

bool isSupportedResponseLanguageCode(const std::string& code);

std::string resolveResponseLanguage(const std::string& configured,
                                    const std::string& system_fallback = "en");

std::string languageFromEnvironment();

std::string responseLanguageDisplayName(const std::string& code);

} // namespace RDK::LLM

#endif
