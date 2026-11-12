#ifndef RDK_ULLM_SESSION_GRAPH_MEMORY_H
#define RDK_ULLM_SESSION_GRAPH_MEMORY_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class URdkDomainAccess;
struct ConversationState;
struct LLMSessionContext;

struct ConnectTemplate {
    std::string from_class;
    std::string to_class;
    std::string from_port;
    std::string to_port;
};

struct ConnectRecord {
    std::string from_long_name;
    std::string to_long_name;
    std::string from_property;
    std::string to_property;
    int64_t created_at_unix_sec = 0;
};

struct LastAddComponentMemory {
    std::string class_name;
    std::string parent_long_name;
    std::string short_name_base;
};

struct SessionGraphMemory {
    std::vector<std::string> added_long_names;
    std::vector<ConnectRecord> linked_records;
    std::optional<ConnectTemplate> last_template;
    /// Last successful add_component args (for «ещё таких же» follow-ups).
    std::optional<LastAddComponentMemory> last_add;
};

nlohmann::json sessionGraphMemoryToJson(const SessionGraphMemory& graph);
SessionGraphMemory sessionGraphMemoryFromJson(const nlohmann::json& j);

void resetSessionGraphMemory(SessionGraphMemory& graph);
/// Clears session graph when project open/close state changes (same chat, different project).
void syncSessionGraphOnSessionChange(ConversationState& state, const LLMSessionContext& session);

void recordSessionAdd(ConversationState& state, const std::string& long_name);
std::string formatConnectKnownFact(const ConnectRecord& rec);
void appendConnectKnownFact(ConversationState& state, const ConnectRecord& rec);
void recordSessionConnect(ConversationState& state, URdkDomainAccess& domain,
                          const nlohmann::json& connect_result, int channel_index);
void recordWriteToolOutcome(ConversationState& state, URdkDomainAccess& domain,
                          const std::string& tool_name, const nlohmann::json& result,
                          int channel_index,
                          const nlohmann::json* arguments = nullptr);
std::vector<std::string> sessionRemainingLongNames(const ConversationState& state);
bool isEndpointLinkedInSession(const SessionGraphMemory& mem, const std::string& long_name);

} // namespace RDK::LLM

#endif
