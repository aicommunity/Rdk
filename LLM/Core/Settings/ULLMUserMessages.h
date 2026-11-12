#ifndef RDK_ULLM_USER_MESSAGES_H
#define RDK_ULLM_USER_MESSAGES_H

#include <map>
#include <string>

namespace RDK::LLM {

std::string formatUserMessage(const std::string& key, const std::string& lang,
                              const std::map<std::string, std::string>& placeholders = {});

} // namespace RDK::LLM

#endif
