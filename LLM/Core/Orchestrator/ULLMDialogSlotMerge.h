#ifndef RDK_ULLM_DIALOG_SLOT_MERGE_H
#define RDK_ULLM_DIALOG_SLOT_MERGE_H

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "../LlmTypes.h"
#include "ULLMLifecycleArgumentGate.h"

namespace RDK::LLM {

class ULLMToolRegistry;

struct DialogSlotMergeInput {
    const std::vector<LLMMessage>* messages = nullptr;
    const PendingToolArguments* pending = nullptr;
    const LLMGuiContextSnapshot* gui = nullptr;
    const ULLMToolRegistry* registry = nullptr;
    std::string inferred_tool_name;
};

struct DialogSlotMergeResult {
    nlohmann::json partial_arguments = nlohmann::json::object();
    bool has_connect_endpoints = false;
    std::optional<std::string> from_class;
    std::optional<std::string> to_class;
    std::string inferred_tool_name;
};

DialogSlotMergeResult mergeDialogSlotsIntoToolArgs(const DialogSlotMergeInput& in);

} // namespace RDK::LLM

#endif
