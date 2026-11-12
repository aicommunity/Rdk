#include <gtest/gtest.h>

#include "Session/ULLMConversationStore.h"
#include "Session/ULLMGuiTurnPin.h"

using namespace RDK::LLM;

TEST(LLMGuiTurnPin, WriteContextUsesPinnedSnapshotNotLaterMutation)
{
    ConversationState state;
    LLMGuiContextSnapshot at_request;
    at_request.diagram_scope_long_name = "Model.Hardware";
    at_request.current_component_long_name = "Model.Hardware";
    beginGuiTurnPin(state, at_request);

    ASSERT_TRUE(state.active_turn_pin.has_value());
    const LLMGuiContextSnapshot* write_ctx = guiContextForWrite(state);
    ASSERT_NE(write_ctx, nullptr);
    EXPECT_EQ(write_ctx->diagram_scope_long_name, "Model.Hardware");

    LLMGuiContextSnapshot later;
    later.diagram_scope_long_name = "Model.Other";
    state.last_gui_context = later;

    write_ctx = guiContextForWrite(state);
    ASSERT_NE(write_ctx, nullptr);
    EXPECT_EQ(write_ctx->diagram_scope_long_name, "Model.Hardware");
}
