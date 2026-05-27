#include "ULLMQuantityParser.h"

#include <cctype>
#include <regex>

namespace RDK::LLM {

namespace {

bool containsWord(const std::string& lower, const std::string& word)
{
    const std::string pattern = "\\b" + word + "\\b";
    return std::regex_search(lower, std::regex(pattern));
}

} // namespace

ParsedQuantity extractQuantity(const std::string& text)
{
    ParsedQuantity out;
    std::string lower = text;
    for(char& c : lower)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    {
        static const std::regex kNumeric(
            R"((\b)(\d+)\s*(x|times|раз|шт)\b)", std::regex::icase);
        std::smatch match;
        if(std::regex_search(text, match, kNumeric) && match[2].matched)
        {
            out.count = std::stoi(match[2].str());
            out.valid = true;
            if(out.count < 1)
                out.count = 1;
            return out;
        }
    }

    if(containsWord(lower, "two") || containsWord(lower, "два") || containsWord(lower, "две")
       || containsWord(lower, "pair") || containsWord(lower, "couple"))
    {
        out.count = 2;
        out.valid = true;
        return out;
    }

    if(containsWord(lower, "three") || containsWord(lower, "три"))
    {
        out.count = 3;
        out.valid = true;
        return out;
    }

    return out;
}

} // namespace RDK::LLM
