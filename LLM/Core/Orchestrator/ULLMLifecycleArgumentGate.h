#ifndef RDK_ULLM_LIFECYCLE_ARGUMENT_GATE_H
#define RDK_ULLM_LIFECYCLE_ARGUMENT_GATE_H

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "../LlmTypes.h"
#include "ULLMConfigurationLifecycle.h"

namespace RDK {
class UApplication;
}

namespace RDK::LLM {

class ULLMToolRegistry;

struct ToolArgumentFieldSpec {
    std::string name;
    std::string type = "string";
    std::string description;
    bool required = true;
};

struct PendingToolArguments {
    std::string tool_name;
    ConfigurationLifecycleAction action = ConfigurationLifecycleAction::None;
    nlohmann::json partial_arguments = nlohmann::json::object();
    std::vector<ToolArgumentFieldSpec> missing_fields;
    int64_t created_at_unix_sec = 0;
};

struct LifecycleArgumentPreflight {
    bool ready = false;
    std::string tool_name;
    nlohmann::json arguments = nlohmann::json::object();
    std::vector<ToolArgumentFieldSpec> missing_fields;
};

/// Extract filesystem path from free-form user text (absolute paths, project.ini).
std::string extractPathFromUserText(const std::string& user_text);

/// Build tool argument specs for a lifecycle action.
std::vector<ToolArgumentFieldSpec> argumentFieldsForLifecycle(ConfigurationLifecycleAction action);

/// Merge follow-up user message into pending partial arguments.
nlohmann::json mergeArgumentsFromUserText(const PendingToolArguments& pending,
                                          const std::string& user_text,
                                          RDK::UApplication* app);

/// Validate merged args for a lifecycle tool; returns missing field specs.
std::vector<ToolArgumentFieldSpec> findMissingLifecycleFields(const std::string& tool_name,
                                                              const nlohmann::json& args,
                                                              RDK::UApplication* app);

LifecycleArgumentPreflight preflightLifecycleArguments(ConfigurationLifecycleAction action,
                                                       const std::string& user_text,
                                                       RDK::UApplication* app);

std::string formatArgumentRequestPrompt(const std::string& tool_name,
                                        ConfigurationLifecycleAction action,
                                        const std::vector<ToolArgumentFieldSpec>& missing,
                                        RDK::UApplication* app);

std::string toolNameForLifecycleAction(ConfigurationLifecycleAction action);

ConfigurationLifecycleAction lifecycleActionFromToolName(const std::string& tool_name);

bool isGraphAddComponentTool(const std::string& tool_name);

std::vector<ToolArgumentFieldSpec> findMissingFieldsFromToolSchema(const LLMToolDefinition& def,
                                                                   const nlohmann::json& args);

std::vector<ToolArgumentFieldSpec> findMissingToolArguments(const std::string& tool_name,
                                                            const nlohmann::json& args,
                                                            const ULLMToolRegistry& registry);

std::vector<ToolArgumentFieldSpec> findMissingArgumentsForTool(const std::string& tool_name,
                                                              const nlohmann::json& args,
                                                              RDK::UApplication* app,
                                                              const ULLMToolRegistry& registry);

} // namespace RDK::LLM

#endif
