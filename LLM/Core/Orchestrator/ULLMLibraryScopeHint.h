#ifndef RDK_ULLM_LIBRARY_SCOPE_HINT_H
#define RDK_ULLM_LIBRARY_SCOPE_HINT_H

#include <optional>
#include <string>
#include <unordered_set>

namespace RDK::LLM {

enum class LibraryScopeHint { None, Pulse, Motion, Hardware };

/// Detect optional library focus from user text (RU/EN keywords). No scope → use full catalog.
LibraryScopeHint detectLibraryScopeFromUserText(const std::string& user_text);

std::string libraryScopeHintManifestSection(LibraryScopeHint hint);

const std::unordered_set<std::string>* componentClassAllowlistForScope(LibraryScopeHint hint);

/// When scope is set, fuzzy-match token to that library's classes; otherwise return trimmed token.
std::string resolveComponentClassName(const std::string& query, LibraryScopeHint scope);

} // namespace RDK::LLM

#endif
