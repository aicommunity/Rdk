#ifndef RDK_LLM_AGENT_SCENARIO_TYPES_H
#define RDK_LLM_AGENT_SCENARIO_TYPES_H

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "LlmTypes.h"
#include "llm_e2e_analyzer.h"

namespace RDK::LLM::AgentScenarios {

enum class AgentScenarioTier { Deterministic, E2e };

enum class RegistryProfile { Core, Full };

struct AgentSessionSpec {
    bool project_loaded = false;
    bool llm_write_enabled = true;
    bool auto_apply_writes = false;
    int active_channel_index = 0;
    std::string autonomous_mode = "off";
};

struct AgentGuiSpec {
    std::string focused_component_long_name;
    std::string focused_class_name;
    std::string project_xml_path;
    int channel_index = 0;
};

struct AgentScenarioExpect {
    std::optional<bool> orchestrator_ok;
    std::optional<bool> no_suitable_tool;
    std::optional<bool> needs_argument_clarification;
    std::optional<bool> needs_entity_clarification;
    std::optional<bool> pending_confirmation;
    std::optional<bool> pending_plan_execution;
    std::vector<std::string> tools_invoked;
    std::vector<std::string> tools_invoked_any;
    std::vector<std::string> tools_invoked_all;
    std::vector<std::string> tools_forbidden;
    std::optional<int> tools_invoked_count_min;
    std::optional<int> tools_invoked_count_max;
    std::vector<std::string> final_text_contains;
    std::vector<std::string> final_text_contains_any;
    std::vector<std::string> final_text_not_contains;
    std::vector<std::string> system_prompt_contains_any;
    std::optional<int> provider_rounds_max;
    std::optional<int> tool_messages_max;
    std::optional<bool> mock_queue_empty;
    std::optional<LLMIntentKind> expect_intent;
    /// Final or transitioned workflow phase name (e.g. Executing, Idle).
    std::optional<std::string> phase_entered;
    std::vector<std::string> audit_events_any;
    std::vector<std::string> audit_events_all;
    std::optional<bool> tool_loop_entered;
    std::optional<std::string> turn_terminal;
};

struct AgentScenarioTurn {
    std::string user_text;
    std::vector<nlohmann::json> mock_script;
    bool confirm_pending = false;
    bool reload_persisted_session = false;
    std::optional<AgentScenarioExpect> expect;
};

struct AgentPreResolvedEntity {
    std::string kind;
    std::string query_key;
    std::string canonical_value;
    int channel_index = 0;
};

struct AgentE2eSpec {
    E2eLab::E2eGoalKind goal = E2eLab::E2eGoalKind::ToolInvoked;
    std::vector<std::string> expected_tools_any;
    std::vector<std::string> forbidden_tools;
    bool require_orchestrator_ok = true;
    bool allow_app_unavailable_result = true;
    /// Use Ollama Native + thinking model (qwen3:14b / NMSDK_LLM_OLLAMA_THINKING_MODEL).
    bool use_thinking_profile = false;
};

struct AgentScenarioCase {
    int schema_version = 1;
    std::string id;
    AgentScenarioTier tier = AgentScenarioTier::Deterministic;
    std::vector<std::string> tags;
    std::string suite;
    RegistryProfile registry_profile = RegistryProfile::Core;
    std::string user_text;
    std::vector<AgentScenarioTurn> turns;
    std::optional<AgentGuiSpec> gui;
    std::vector<AgentPreResolvedEntity> pre_resolved_entities;
    AgentSessionSpec session;
    std::map<std::string, std::string> env;
    std::optional<LLMIntentKind> expect_intent;
    std::vector<nlohmann::json> mock_script;
    bool confirm_pending = false;
    AgentScenarioExpect expect;
    std::optional<AgentE2eSpec> e2e;
    std::string source_path;
};

struct AgentScenarioRun {
    LLMFinalResponse final_response;
    E2eLab::E2eConversationDigest digest;
    std::string ephemeral_system_text;
    size_t provider_invoke_count = 0;
    size_t mock_queue_remaining = 0;
    size_t tool_message_count = 0;
    std::string final_workflow_phase;
    std::vector<std::string> audit_event_types;
};

struct AgentScenarioVerdict {
    bool passed = false;
    std::string diagnosis;
};

RegistryProfile parseRegistryProfile(const std::string& s);
AgentScenarioTier parseTier(const std::string& s);
std::optional<LLMIntentKind> parseIntentKind(const std::string& s);

} // namespace RDK::LLM::AgentScenarios

#endif
