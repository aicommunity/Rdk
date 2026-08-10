#ifndef RDK_ULLM_RECORDED_TOOL_INVOKE_H
#define RDK_ULLM_RECORDED_TOOL_INVOKE_H

#include <functional>
#include <string>

#include <nlohmann/json.hpp>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"
#include "ULLMWriteToolExecution.h"

namespace RDK::LLM {

class ULLMSystemLogReader;

struct RecordedToolInvokeRequest {
    std::string session_id;
    std::string trace_id;
    std::string tool_name;
    nlohmann::json arguments = nlohmann::json::object();
    LLMSessionContext session;
    LLMGuiContextSnapshot gui;
    std::string user_text_hint;
    std::string user_lang = "en";
    std::string idempotency_action_id;
    std::string tool_call_id; // empty → generate rec-<sha>
    bool skip_preview = false;
    bool confirmed = false;
    bool append_outcome_assistant = true;
    /// When true (Task/Plan), force confirmed gateway invoke even if write_exec is unset.
    bool force_confirmed = false;
    /// Caller will record turn tool_trace on ConversationState (avoid gateway double-append).
    bool skip_turn_tool_trace = false;
};

struct RecordedToolInvokeResult {
    ToolGatewayResult gateway;
    std::string tool_call_id;
    std::string preview_text;
    std::string outcome_text;
    bool needs_hitl = false;
    bool wrote_assistant_tool_calls = false;
    bool wrote_tool_result = false;
};

struct RecordedToolInvokeDeps {
    ULLMToolRegistry& registry;
    ULLMToolGateway& gateway;
    ULLMConversationStore& store;
    /// When set, Write tools use preview/verify path (orchestrator).
    std::function<WriteToolExecutionResult(ConversationState&, WriteToolExecutionRequest&)> write_exec;
    ULLMSystemLogReader* log_reader = nullptr;
};

/// Memory-correct tool invoke for all non-ReAct callers (DD-MEM-001 / TD-150).
RecordedToolInvokeResult recordedToolInvoke(ConversationState& state,
                                            const RecordedToolInvokeDeps& deps,
                                            const RecordedToolInvokeRequest& req);

} // namespace RDK::LLM

#endif
