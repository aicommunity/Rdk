#ifndef RDK_ULLM_CHAT_NAME_COMPLETION_H
#define RDK_ULLM_CHAT_NAME_COMPLETION_H

#include <cstddef>
#include <string>
#include <vector>

namespace RDK::LLM {

struct ChatNameTokenSpan {
    size_t begin = 0; // byte offset in UTF-8 text
    size_t end = 0;   // exclusive
    std::string text;
};

/// Extract [A-Za-z0-9_.]+ token under cursor (cursor_pos is byte index, clamp to size).
ChatNameTokenSpan extractChatNameToken(const std::string& text, size_t cursor_pos);

/// Prefix-rank candidates (case-insensitive). Exact prefix first, then contains.
std::vector<std::string> rankChatNameCompletions(const std::string& prefix,
                                                 const std::vector<std::string>& dictionary,
                                                 size_t max_results = 32);

/// After "Component." or "Component:" prefer names starting with that parent scope.
std::vector<std::string> filterCompletionsForScopedToken(
    const std::string& token,
    const std::vector<std::string>& long_names,
    const std::vector<std::string>& class_names,
    const std::vector<std::string>& property_names,
    size_t max_results = 32);

} // namespace RDK::LLM

#endif
