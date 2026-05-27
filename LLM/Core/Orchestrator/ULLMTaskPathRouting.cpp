#include "ULLMTaskPathRouting.h"

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
                                LLMAutonomousMode autonomous_mode)
{
    TaskPathDecision out;
    out.quantity = extractQuantity(text_en);

    if(intent != LLMIntentKind::Mutate)
        return out;

    if(out.quantity.count > 1)
        out.use_task_path = true;
    if(countMutateVerbs(text_en) >= 2)
        out.use_task_path = true;
    if(autonomous_mode != LLMAutonomousMode::Off)
        out.use_task_path = true;
    if(hasSequenceMarker(text_en))
        out.use_task_path = true;

    return out;
}

} // namespace RDK::LLM
