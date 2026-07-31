#ifndef RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H
#define RDK_ULLM_WRITE_ARGUMENT_NORMALIZER_H

#include "ULLMNameResolution.h"

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace RDK::LLM {
struct LLMGuiContextSnapshot;
}

namespace RDK::LLM {

class URdkDomainAccess;

struct WriteArgumentNormalizeResult {
    bool ok = false;
    bool needs_clarification = false;
    std::string error_code;
    std::string message;
    nlohmann::json normalized_arguments;
    nlohmann::json clarification = nlohmann::json::object();
};

bool writeToolNeedsEntityResolution(const std::string& tool_name);

bool isNetGraphWriteTool(const std::string& tool_name);

struct ConversationState;

WriteArgumentNormalizeResult normalizeWriteToolArguments(const std::string& tool_name,
                                                         nlohmann::json arguments,
                                                         URdkDomainAccess& domain,
                                                         int channel_index,
                                                         const std::string& user_text = "",
                                                         const ConversationState* conversation = nullptr);

struct PreparedAddComponentInvoke {
    nlohmann::json arguments;
    int repeat_count = 1;
    bool needs_clarification = false;
    nlohmann::json clarification = nlohmann::json::object();
};

struct SessionGraphMemory;

/// True when user asks to add more of the same (ещё/таких же/same/more).
bool looksLikeRepeatSameAddCue(const std::string& user_text);

/// When user names a registered class (or repeat-cue «таких же» with last_add) and asks to add.
std::optional<PreparedAddComponentInvoke> tryPrepareAddComponentDirect(
    const std::string& user_text, const LLMGuiContextSnapshot& gui, URdkDomainAccess& domain,
    int channel_index, int repeat_count, const SessionGraphMemory* session_graph = nullptr);

} // namespace RDK::LLM

#endif
