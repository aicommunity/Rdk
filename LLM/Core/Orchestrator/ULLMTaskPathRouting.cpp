#include "ULLMTaskPathRouting.h"

#include "../Session/ULLMConversationStore.h"
#include "ULLMConnectPlanParsing.h"
#include "ULLMTaskPlanParsing.h"

#include <regex>

namespace RDK::LLM {

namespace {

int countMutateVerbs(const std::string& text_en)
{
    static const std::regex kVerb(
        R"(\b(add|connect|link|remove|delete|set)\b)", std::regex::icase);
    int count = 0;
    for(std::sregex_iterator it(text_en.begin(), text_en.end(), kVerb), end; it != end; ++it)
        ++count;
    return count;
}

bool hasSequenceMarker(const std::string& text_en)
{
    static const std::regex kSeq(
        R"((\band then\b|\bthen\b|после чего))", std::regex::icase);
    return std::regex_search(text_en, kSeq);
}

} // namespace

TaskPathDecision decideTaskPath(const std::string& text_en, LLMIntentKind intent,
                                LLMAutonomousMode autonomous_mode,
                                const ConversationState* state)
{
    TaskPathDecision out;
    if(state && state->last_quantity.valid)
    {
        out.quantity.count = state->last_quantity.primary;
        out.quantity.valid = true;
    }
    else
    {
        out.quantity = extractQuantityHeuristic(text_en);
    }

    if(intent != LLMIntentKind::Mutate)
        return out;

    if(out.quantity.count > 1)
        out.use_task_path = true;
    if(extractClassAddSpecsFromGoal(text_en).size() >= 2)
        out.use_task_path = true;
    if(countMutateVerbs(text_en) >= 2)
        out.use_task_path = true;
    if(autonomous_mode != LLMAutonomousMode::Off)
        out.use_task_path = true;
    if(hasSequenceMarker(text_en))
        out.use_task_path = true;
    const ParsedConnectGoal connect = parseConnectGoal(text_en);
    if((connect.kind == ConnectGoalKind::RemainingSessionDelta
        || connect.kind == ConnectGoalKind::AnalogousToPrevious)
       || connect.explicit_links.size() >= 2
       || (isConnectGoalText(text_en) && connect.link_count > 1))
        out.use_task_path = true;

    return out;
}

} // namespace RDK::LLM
