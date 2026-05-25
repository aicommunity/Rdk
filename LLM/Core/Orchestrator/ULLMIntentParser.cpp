#include "ULLMIntentParser.h"

#include <cctype>

namespace RDK::LLM {

static bool containsAny(const std::string& text, const std::vector<const char*>& words)
{
    for(const char* w : words)
    {
        if(text.find(w) != std::string::npos)
            return true;
    }
    return false;
}

LLMIntentKind ULLMIntentParser::parse(const std::string& user_text) const
{
    std::string lower = user_text;
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if(containsAny(lower, {"добав", "создай", "удали", "измени", "сохран", "загруз", "add ", "create ",
                           "remove ", "delete ", "save ", "load "}))
        return LLMIntentKind::Mutate;
    if(containsAny(lower, {"почему", "объясни", "explain", "why "}))
        return LLMIntentKind::Explain;
    if(containsAny(lower, {"что", "какие", "покажи", "список", "опиши", "what", "list", "show", "describe"}))
        return LLMIntentKind::Query;
    return LLMIntentKind::Query;
}

} // namespace RDK::LLM
