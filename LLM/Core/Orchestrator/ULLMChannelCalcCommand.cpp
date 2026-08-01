#include "ULLMChannelCalcCommand.h"

#include <QString>

namespace RDK::LLM {

namespace {

std::string toLower(const std::string& s)
{
    QString q = QString::fromUtf8(s.data(), static_cast<int>(s.size())).toLower();
    // Normalize ё→е so «расчёт» matches «расчет» / «расч» stems.
    q.replace(QString::fromUtf8("ё"), QString::fromUtf8("е"));
    return q.toUtf8().toStdString();
}

bool contains(const std::string& hay, const char* needle)
{
    return hay.find(needle) != std::string::npos;
}

bool mentionsCalc(const std::string& lower)
{
    return contains(lower, "расчет") || contains(lower, "расчёт") || contains(lower, "расч")
           || contains(lower, "calculation") || contains(lower, "calc ")
           || contains(lower, " calc") || lower == "calc"
           || contains(lower, "симуляц") || contains(lower, "simulation")
           || contains(lower, "вычисл");
}

} // namespace

ChannelCalcAction detectChannelCalcAction(const std::string& user_text)
{
    const std::string lower = toLower(user_text);
    if(lower.empty())
        return ChannelCalcAction::None;

    // Compact RU stems already used by intent parser.
    if(contains(lower, "запусти расч") || contains(lower, "запустить расч")
       || contains(lower, "старт расч"))
        return ChannelCalcAction::Start;
    if(contains(lower, "останови расч") || contains(lower, "остановить расч")
       || contains(lower, "пауза расч"))
        return ChannelCalcAction::Pause;

    const bool calc = mentionsCalc(lower);
    if(!calc && !contains(lower, "channel calculation") && !contains(lower, "channel calc"))
        return ChannelCalcAction::None;

    if(contains(lower, "reset") || contains(lower, "сброс") || contains(lower, "сбрось")
       || contains(lower, "перезапуст"))
        return ChannelCalcAction::Reset;

    if(contains(lower, "step") || contains(lower, "шаг") || contains(lower, "шаговый"))
        return ChannelCalcAction::Step;

    if(contains(lower, "pause") || contains(lower, "пауза") || contains(lower, "останови")
       || contains(lower, "стоп") || contains(lower, "stop "))
        return ChannelCalcAction::Pause;

    if(contains(lower, "start") || contains(lower, "run ") || contains(lower, "запуст")
       || contains(lower, "старт") || contains(lower, "посчитай") || contains(lower, "посчит"))
        return ChannelCalcAction::Start;

    return ChannelCalcAction::None;
}

bool isChannelCalcGoalText(const std::string& user_text)
{
    return detectChannelCalcAction(user_text) != ChannelCalcAction::None;
}

const char* toolNameForChannelCalcAction(ChannelCalcAction action)
{
    switch(action)
    {
    case ChannelCalcAction::Start:
        return "start_channel_calculation";
    case ChannelCalcAction::Pause:
        return "pause_channel_calculation";
    case ChannelCalcAction::Reset:
        return "reset_channel_calculation";
    case ChannelCalcAction::Step:
        return "step_channel_calculation";
    default:
        return "";
    }
}

int channelIndexForCalcRequest(const std::string& user_text, int active_channel_index)
{
    const std::string lower = toLower(user_text);
    if(contains(lower, "текущ") || contains(lower, "этот канал") || contains(lower, "этого канала")
       || contains(lower, "active channel") || contains(lower, "this channel")
       || contains(lower, "current channel"))
    {
        if(active_channel_index >= 0)
            return active_channel_index;
    }
    return -1;
}

std::string formatChannelCalcUserMessage(ChannelCalcAction action, const ToolGatewayResult& tr,
                                         int channel_index)
{
    if(!tr.ok)
    {
        if(!tr.message.empty())
            return tr.message;
        return std::string("Operation failed: ") + toolNameForChannelCalcAction(action);
    }

    const std::string scope =
        channel_index < 0 ? "all channels" : ("channel " + std::to_string(channel_index));
    switch(action)
    {
    case ChannelCalcAction::Start:
        return "Calculation started (" + scope + ").";
    case ChannelCalcAction::Pause:
        return "Calculation paused (" + scope + ").";
    case ChannelCalcAction::Reset:
        return "Calculation reset (" + scope + ").";
    case ChannelCalcAction::Step:
        return "Calculation stepped once (" + scope + ").";
    default:
        return "Change applied.";
    }
}

} // namespace RDK::LLM
