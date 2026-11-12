#ifndef RDK_ULLM_WRITE_TOOL_EXECUTION_H
#define RDK_ULLM_WRITE_TOOL_EXECUTION_H

#include <string>

#include <nlohmann/json.hpp>

#include "../LlmTypes.h"
#include "../Tools/ULLMToolGateway.h"
#include "ULLMActionIntentPreview.h"

namespace RDK::LLM {

class ULLMAgentOrchestrator;
struct ConversationState;

struct WriteToolExecutionRequest {
    std::string session_id;
    std::string trace_id;
    std::string tool_name;
    nlohmann::json arguments;
    LLMSessionContext session;
    LLMGuiContextSnapshot gui;
    std::string user_lang;
    std::string user_text_hint;
    std::string idempotency_action_id;
    bool skip_preview = false;
    bool confirmed = false;
    const LinkPreviewContext* link_ctx = nullptr;
};

struct WriteToolExecutionResult {
    ToolGatewayResult gateway;
    std::string preview_text;
    std::string outcome_text;
    std::string verify_detail;
    bool verify_ok = true;
    bool needs_hitl = false;
};

std::string formatHitlConfirmationText(const std::string& preview_text, const std::string& lang);

std::string combinePreviewAndOutcome(const std::string& preview_text, const std::string& outcome_text);

WriteToolExecutionResult executeWriteWithPreviewAndVerify(ULLMAgentOrchestrator& orch,
                                                          ConversationState& state,
                                                          const WriteToolExecutionRequest& req);

} // namespace RDK::LLM

#endif
