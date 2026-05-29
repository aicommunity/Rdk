#ifndef RDK_LLM_TYPES_H
#define RDK_LLM_TYPES_H

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
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

enum class LLMIntentKind { Auto, Query, Mutate, Explain, Plan };

enum class LLMTaskPathMode { HintOnly, FastPath };

/// Agent v2: explicit turn outcome for UI/telemetry (see Unified-Turn-Contract.md).
enum class TurnTerminal {
    Completed,
    MaxRounds,
    Cancelled,
    ProviderError,
    AwaitingUser,
    AwaitingConfirm,
    TaskFastPathCompleted
};

inline const char* turnTerminalName(TurnTerminal terminal)
{
    switch(terminal)
    {
    case TurnTerminal::Completed:
        return "Completed";
    case TurnTerminal::MaxRounds:
        return "MaxRounds";
    case TurnTerminal::Cancelled:
        return "Cancelled";
    case TurnTerminal::ProviderError:
        return "ProviderError";
    case TurnTerminal::AwaitingUser:
        return "AwaitingUser";
    case TurnTerminal::AwaitingConfirm:
        return "AwaitingConfirm";
    case TurnTerminal::TaskFastPathCompleted:
        return "TaskFastPathCompleted";
    }
    return "Completed";
}

/// Scenario D: configurable multi-step autonomy (Post-MVP). Default Off.
enum class LLMAutonomousMode { Off, Strict, SemiAuto };

enum class LLMPresentationEffect {
    None,
    ContextOnly,
    DiagramRefresh,
    FullShellRefresh
};

enum class LLMUiPanel {
    None,
    ComponentsList,
    Channels,
    Logger,
    Watch,
    Images,
    ProjectDescription,
    Profiling,
    Diagram, // refresh only
    ComponentGuiTabHost
};

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

struct LLMPresentationEvent {
    LLMPresentationEffect effect = LLMPresentationEffect::None;
    std::string configuration_ini_path;
    bool project_loaded = false;
    bool update_context = false;
    bool project_closed = false;
    bool add_to_recent = false;
    LLMUiPanel show_panel = LLMUiPanel::None;
    bool show_panel_visible = true;
    int select_active_channel = -1;
};

struct ApplicationCommandResult {
    DomainStatus status;
    nlohmann::json payload = nlohmann::json::object();
    LLMPresentationEffect presentation = LLMPresentationEffect::None;
    bool update_context = false;
    bool project_closed = false;
    bool add_to_recent = false;
    std::string resolved_configuration_path;
    LLMUiPanel show_panel = LLMUiPanel::None;
    bool show_panel_visible = true;
    int select_active_channel = -1;
};

struct LLMProviderCapabilities {
    bool supports_tool_calling = true;
    bool supports_streaming = true;
    bool supports_strict_json_schema = false;
    bool runs_in_process = false;
    bool requires_network = true;
};

enum class OllamaChatTemplateFamily {
    Auto,
    Qwen2,
    Llama3,
    Mistral,
    Gemma2,
    ChatML
};

struct LLMProviderProfile {
    std::string profile_id = "ollama-local";
    LLMProviderKind kind = LLMProviderKind::OllamaOpenAICompat;
    std::string base_url = "http://127.0.0.1:11434/v1";
    std::string model = "qwen2.5:7b";
    std::string api_key;
    /// Environment variable name for Bearer token (e.g. DEEPSEEK_API_KEY).
    std::string api_key_env;
    bool is_cloud = false;
    bool prefer_local = true;
    /// Empty = detect from model name (qwen2.5 → Qwen2, llama3 → Llama3, …).
    OllamaChatTemplateFamily chat_template = OllamaChatTemplateFamily::Auto;
};

struct LLMProfileEndpointOverride {
    std::string base_url;
    std::string model;
};

enum class LLMSendShortcutMode {
    CtrlEnter,
    Enter
};

struct LLMRuntimeProviderSettings {
    std::string active_profile_id = "ollama-local";
    bool allow_cloud_providers = false;
    bool llm_write_enabled = true;
    /// When true, write tools run immediately without per-step Apply confirmation.
    bool llm_auto_apply_writes = false;
    LLMAutonomousMode autonomous_mode = LLMAutonomousMode::Off;
    int max_autonomous_steps = 3;
    bool translate_queries_to_en = true;
    /// When true, use a short LLM JSON call if heuristic quantity extraction fails.
    bool quantity_llm_fallback = false;
    /// Optional fallback for connect-plan construction when deterministic builder fails.
    bool connect_plan_llm_fallback = false;
    /// Task executor fast-path (default hint-only; fast_path mirrors CI strict mode).
    LLMTaskPathMode task_path_mode = LLMTaskPathMode::HintOnly;
    std::map<std::string, std::string> api_keys_by_profile_id;
    /// Per-profile endpoint overrides (empty fields → use built-in preset).
    std::map<std::string, LLMProfileEndpointOverride> endpoint_overrides_by_profile_id;
    /// Empty = Auto (system locale → supported code → en).
    std::string preferred_response_language;
    LLMSendShortcutMode send_shortcut = LLMSendShortcutMode::CtrlEnter;
};

struct LLMGuiContextSnapshot {
    int channel_index = 0;
    std::string project_xml_path;
    std::string focused_component_long_name;
    std::string focused_class_name;
    int64_t snapshot_fingerprint = 0;
};

struct LLMSessionContext {
    std::string session_id;
    std::string user_name;
    int user_id = 0;
    bool project_loaded = false;
    bool llm_write_enabled = true;
    bool auto_apply_writes = false;
    LLMAutonomousMode autonomous_mode = LLMAutonomousMode::Off;
    int autonomous_steps_taken = 0;
    bool allow_cloud_llm = false;
    bool allow_save = true;
    int active_channel_index = 0;
};

struct LLMToolCall {
    std::string id;
    std::string name;
    nlohmann::json arguments;
};

struct LLMMessage {
    enum class Role { System, User, Assistant, Tool };
    Role role = Role::User;
    std::string content;
    std::optional<std::string> tool_call_id;
    std::optional<std::string> tool_name;
    std::optional<nlohmann::json> tool_arguments;
    std::optional<nlohmann::json> tool_result;
    /// Set on assistant turns that invoked tools (required for Ollama/OpenAI tool loops).
    std::optional<std::vector<LLMToolCall>> assistant_tool_calls;
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
    /// OpenAI-compatible `tool_choice` (e.g. force `create_configuration`).
    std::optional<nlohmann::json> tool_choice;
    int max_tokens = 4096;
    float temperature = 0.2f;
    bool stream = false;
    /// OpenAI-compatible `response_format` (e.g. json_schema for Plan intent).
    std::optional<nlohmann::json> response_format;
    /// Resolved BCP47-like code (en, ru, …). Empty → providers use "en".
    std::string response_language;
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
    bool requires_project_loaded = true;
};

struct ToolInvokeRequest {
    std::string trace_id;
    std::string tool_name;
    nlohmann::json arguments;
    std::string idempotency_key;
    LLMSessionContext session;
    std::string confirmation_id;
    bool confirmed = false;
    /// Current user turn text (for add_component class inference when the model picks a wrong class).
    std::string user_text_hint;
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
    std::optional<std::unordered_set<std::string>> allowed_tool_names;
    std::string focus_class_name;
};

} // namespace RDK::LLM

#endif
