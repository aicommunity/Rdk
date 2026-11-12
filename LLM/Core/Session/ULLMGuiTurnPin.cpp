#include "ULLMGuiTurnPin.h"

#include "ULLMConversationStore.h"

namespace RDK::LLM {

void beginGuiTurnPin(ConversationState& state, const LLMGuiContextSnapshot& gui_at_request)
{
    GuiTurnPin pin;
    pin.at_user_message = gui_at_request;
    if(pin.at_user_message.current_component_long_name.empty()
       && !pin.at_user_message.focused_component_long_name.empty())
        pin.at_user_message.current_component_long_name =
            pin.at_user_message.focused_component_long_name;
    state.active_turn_pin = pin;
}

const LLMGuiContextSnapshot* guiContextForWrite(const ConversationState& state)
{
    if(state.active_turn_pin)
        return &state.active_turn_pin->at_user_message;
    if(state.last_gui_context)
        return &*state.last_gui_context;
    return nullptr;
}

void recordWriteInvokeGui(ConversationState& state, const LLMGuiContextSnapshot& gui_at_invoke)
{
    if(!state.active_turn_pin)
        return;
    state.active_turn_pin->at_last_write_invoke = gui_at_invoke;
    state.active_turn_pin->has_last_write_invoke = true;
}

void clearGuiTurnPin(ConversationState& state)
{
    state.active_turn_pin.reset();
}

} // namespace RDK::LLM
