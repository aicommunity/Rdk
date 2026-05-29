#include "ULLMQuantityParser.h"

#include <cctype>
#include <regex>
#include <vector>

namespace RDK::LLM {

namespace {

bool containsWord(const std::string& lower, const std::string& word)
{
    const std::string pattern = "\\b" + word + "\\b";
    return std::regex_search(lower, std::regex(pattern));
}

bool containsDelimitedToken(const std::string& lower, const std::string& token)
{
    if(token.empty())
        return false;
    size_t pos = 0;
    while((pos = lower.find(token, pos)) != std::string::npos)
    {
        const size_t end = pos + token.size();
        const bool start_ok =
            pos == 0 || std::isspace(static_cast<unsigned char>(lower[pos - 1]));
        const bool end_ok = end >= lower.size()
                            || std::isspace(static_cast<unsigned char>(lower[end]));
        if(start_ok && end_ok)
            return true;
        pos = end + 1;
    }
    return false;
}

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

struct WordQuantity {
    const char* token;
    int count;
    bool counts_as_explicit;
};

bool tryWordQuantity(const std::string& lower, ParsedQuantity& out)
{
    static const std::vector<WordQuantity> kWords = {
        {"twenty", 20, true},    {"двадцать", 20, true},
        {"nineteen", 19, true},  {"девятнадцать", 19, true},
        {"eighteen", 18, true},  {"восемнадцать", 18, true},
        {"seventeen", 17, true}, {"семнадцать", 17, true},
        {"sixteen", 16, true},   {"шестнадцать", 16, true},
        {"fifteen", 15, true},   {"пятнадцать", 15, true},
        {"fourteen", 14, true},  {"четырнадцать", 14, true},
        {"thirteen", 13, true},  {"тринадцать", 13, true},
        {"twelve", 12, true},    {"двенадцать", 12, true},
        {"eleven", 11, true},    {"одиннадцать", 11, true},
        {"ten", 10, true},       {"десять", 10, true},
        {"nine", 9, true},       {"девять", 9, true},
        {"eight", 8, true},      {"восемь", 8, true},
        {"seven", 7, true},     {"семь", 7, true},
        {"six", 6, true},       {"шесть", 6, true},
        {"five", 5, true},      {"пять", 5, true},
        {"four", 4, true},      {"четыре", 4, true},
        {"three", 3, true},     {"три", 3, true},
        {"two", 2, true},       {"два", 2, true}, {"две", 2, true},
        {"couple", 2, true},    {"pair", 2, true},
    };

    int best = 0;
    bool best_explicit = false;
    for(const WordQuantity& w : kWords)
    {
        const bool ascii = (w.token[0] & 0x80) == 0;
        const bool hit = ascii ? containsWord(lower, w.token)
                               : containsDelimitedToken(lower, w.token);
        if(hit && w.count > best)
        {
            best = w.count;
            best_explicit = w.counts_as_explicit;
        }
    }
    if(best <= 0 || !best_explicit)
        return false;
    out.count = best;
    out.valid = true;
    out.source = QuantityHeuristicSource::Word;
    return true;
}

bool tryNumericSuffix(const std::string& text, ParsedQuantity& out)
{
    static const std::regex kNumeric(
        R"((\b)(\d+)\s*(x|times|раз|шт)\b)", std::regex::icase);
    std::smatch match;
    if(std::regex_search(text, match, kNumeric) && match[2].matched)
    {
        out.count = std::stoi(match[2].str());
        out.valid = true;
        out.source = QuantityHeuristicSource::NumericSuffix;
        if(out.count < 1)
            out.count = 1;
        return true;
    }
    return false;
}

bool hasMutateVerbCue(const std::string& lower)
{
    static const char* kMutateVerbs[] = {"add", "remove", "delete", "connect", "link", "create",
                                         nullptr};
    for(const char** p = kMutateVerbs; *p; ++p)
    {
        if(containsWord(lower, *p))
            return true;
    }
    if(lower.find("добав") != std::string::npos || lower.find("удал") != std::string::npos)
        return true;
    return false;
}

bool tryNumericBare(const std::string& text, const std::string& lower, ParsedQuantity& out)
{
    if(!hasMutateVerbCue(lower))
        return false;

    static const std::regex kBare(R"(\b(\d{1,3})\b)");
    int best = 0;
    for(std::sregex_iterator it(text.begin(), text.end(), kBare), end; it != end; ++it)
    {
        const int n = std::stoi(it->str(1));
        if(n > best)
            best = n;
    }
    (void)lower;
    if(best < 1)
        return false;
    out.count = best;
    out.valid = true;
    out.source = QuantityHeuristicSource::NumericBare;
    return true;
}

} // namespace

bool looksLikeQuantityCue(const std::string& text)
{
    if(extractQuantityHeuristic(text).valid)
        return true;

    const std::string lower = toLowerAscii(text);
    if(hasMutateVerbCue(lower))
        return true;
    if(containsWord(lower, "times") || containsDelimitedToken(lower, "раз")
       || containsDelimitedToken(lower, "шт"))
        return true;
    return false;
}

ParsedQuantity extractQuantityHeuristic(const std::string& text)
{
    ParsedQuantity out;
    if(text.empty())
        return out;

    const std::string lower = toLowerAscii(text);

    if(tryNumericSuffix(text, out))
        return out;
    if(tryWordQuantity(lower, out))
        return out;
    if(tryNumericBare(text, lower, out))
        return out;

    return out;
}

} // namespace RDK::LLM
