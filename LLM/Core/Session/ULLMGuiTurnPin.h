#ifndef RDK_ULLM_GUI_TURN_PIN_H
#define RDK_ULLM_GUI_TURN_PIN_H

#include "../LlmTypes.h"

namespace RDK::LLM {

struct ConversationState;

struct GuiTurnPin {
    LLMGuiContextSnapshot at_user_message;
    LLMGuiContextSnapshot at_last_write_invoke;
    bool has_last_write_invoke = false;
};

void beginGuiTurnPin(ConversationState& state, const LLMGuiContextSnapshot& gui_at_request);

const LLMGuiContextSnapshot* guiContextForWrite(const ConversationState& state);

void recordWriteInvokeGui(ConversationState& state, const LLMGuiContextSnapshot& gui_at_invoke);

void clearGuiTurnPin(ConversationState& state);

} // namespace RDK::LLM

#endif
