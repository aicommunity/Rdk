#ifndef RDK_LLM_TYPES_H
#define RDK_LLM_TYPES_H

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

constexpr const char* TOOL_REGISTRY_VERSION = "1.0.0";
constexpr const char* PROMPT_BUNDLE_ID = "rdk-llm-prompts-1.0.0";

enum class LLMProviderKind {
    OllamaOpenAICompat,
    OpenAICompat,
    OllamaNative,
    EmbeddedLlama,
    Custom,
    Mock
};

enum class LLMToolKind { Read, Write };

enum class LLMIntentKind { Auto, Query, Mutate, Explain };

enum class DomainStatusCode {
    Ok,
    NotInitialized,
    ProjectNotLoaded,
    ComponentNotFound,
    AmbiguousComponent,
    ClassNotFound,
    PropertyNotFound,
    InvalidPropertyValue,
    LinkFailed,
    PolicyDenied,
    IOError
};

struct DomainStatus {
    DomainStatusCode code = DomainStatusCode::Ok;
    std::string message;
    bool ok() const { return code == DomainStatusCode::Ok; }
};

struct LLMProviderCapabilities {
    bool supports_tool_calling = true;
    bool supports_streaming = true;
    bool supports_strict_json_schema = false;
    bool runs_in_process = false;
    bool requires_network = true;
};

struct LLMProviderProfile {
    std::string profile_id = "ollama-local";
    LLMProviderKind kind = LLMProviderKind::OllamaOpenAICompat;
    std::string base_url = "http://127.0.0.1:11434/v1";
    std::string model = "qwen2.5:7b";
    std::string api_key;
    bool prefer_local = true;
};

struct LLMSessionContext {
    std::string session_id;
    std::string user_name;
    int user_id = 0;
    bool project_loaded = false;
    bool llm_write_enabled = true;
    bool allow_cloud_llm = false;
    bool allow_save = true;
    int active_channel_index = 0;
};

struct LLMMessage {
    enum class Role { System, User, Assistant, Tool };
    Role role = Role::User;
    std::string content;
    std::optional<std::string> tool_call_id;
    std::optional<std::string> tool_name;
    std::optional<nlohmann::json> tool_arguments;
    std::optional<nlohmann::json> tool_result;
};

struct LLMToolCall {
    std::string id;
    std::string name;
    nlohmann::json arguments;
};

struct LLMCompletionResult {
    bool ok = true;
    std::string text;
    std::vector<LLMToolCall> tool_calls;
    std::string error_message;
    int prompt_tokens = 0;
    int completion_tokens = 0;
};

struct LLMCompletionOptions {
    std::vector<nlohmann::json> tools_for_api;
    int max_tokens = 4096;
    float temperature = 0.2f;
    bool stream = false;
};

using LLMStreamCallback = std::function<void(const std::string& chunk)>;

struct LLMToolDefinition {
    std::string name;
    std::string version = "1.0.0";
    LLMToolKind kind = LLMToolKind::Read;
    std::string description;
    nlohmann::json input_schema;
    nlohmann::json output_schema;
    bool strict = true;
    bool requires_confirmation = false;
    bool idempotent = false;
};

struct ToolInvokeRequest {
    std::string trace_id;
    std::string tool_name;
    nlohmann::json arguments;
    std::string idempotency_key;
    LLMSessionContext session;
    std::string confirmation_id;
    bool confirmed = false;
};

struct ToolGatewayResult {
    bool ok = false;
    nlohmann::json result;
    std::string error_code;
    std::string message;
    bool pending_confirmation = false;
    std::string confirmation_id;
};

struct PolicyDecision {
    bool allowed = true;
    std::string deny_code;
    std::string deny_message;
};

struct ToolFilter {
    LLMIntentKind intent = LLMIntentKind::Auto;
    bool include_write = false;
    std::string focus_class_name;
};

} // namespace RDK::LLM

#endif
