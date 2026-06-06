#ifndef RDK_ULLM_TASK_PATH_ROUTING_H
#define RDK_ULLM_TASK_PATH_ROUTING_H

#include "../LlmTypes.h"
#include "ULLMQuantityParser.h"

#include <string>

namespace RDK::LLM {

struct ConversationState;

struct TaskPathDecision {
    bool use_task_path = false;
    bool force_plan_intent = false;
    ParsedQuantity quantity;
};

TaskPathDecision decideTaskPath(const std::string& text_en, LLMIntentKind intent,
                                LLMAutonomousMode autonomous_mode,
                                const ConversationState* state = nullptr);

} // namespace RDK::LLM

#endif
