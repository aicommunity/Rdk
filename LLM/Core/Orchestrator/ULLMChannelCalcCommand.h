#ifndef RDK_ULLM_CHANNEL_CALC_COMMAND_H
#define RDK_ULLM_CHANNEL_CALC_COMMAND_H

#include "../LlmTypes.h"

#include <string>

namespace RDK::LLM {

enum class ChannelCalcAction {
    None,
    Start,
    Pause,
    Reset,
    Step,
};

/// Detect start/pause/reset/step channel calculation from RU/EN user text.
ChannelCalcAction detectChannelCalcAction(const std::string& user_text);

bool isChannelCalcGoalText(const std::string& user_text);

const char* toolNameForChannelCalcAction(ChannelCalcAction action);

/// Prefer active session channel when user says «этот/текущий»; else -1 (all).
int channelIndexForCalcRequest(const std::string& user_text, int active_channel_index);

std::string formatChannelCalcUserMessage(ChannelCalcAction action, const ToolGatewayResult& tr,
                                         int channel_index);

} // namespace RDK::LLM

#endif
