#include "ULLMChatNameCompletion.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

bool isTokenChar(unsigned char c)
{
    return std::isalnum(c) || c == '_' || c == '.';
}

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool startsWithCi(const std::string& s, const std::string& prefix)
{
    if(prefix.size() > s.size())
        return false;
    for(size_t i = 0; i < prefix.size(); ++i)
    {
        const char a = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
        const char b = static_cast<char>(std::tolower(static_cast<unsigned char>(prefix[i])));
        if(a != b)
            return false;
    }
    return true;
}

bool containsCi(const std::string& s, const std::string& needle)
{
    if(needle.empty())
        return true;
    const std::string sl = toLowerAscii(s);
    const std::string nl = toLowerAscii(needle);
    return sl.find(nl) != std::string::npos;
}

void pushUnique(std::vector<std::string>& out, const std::string& s)
{
    if(s.empty())
        return;
    if(std::find(out.begin(), out.end(), s) != out.end())
        return;
    out.push_back(s);
}

} // namespace

ChatNameTokenSpan extractChatNameToken(const std::string& text, size_t cursor_pos)
{
    ChatNameTokenSpan span;
    if(text.empty())
        return span;
    if(cursor_pos > text.size())
        cursor_pos = text.size();

    size_t begin = cursor_pos;
    while(begin > 0 && isTokenChar(static_cast<unsigned char>(text[begin - 1])))
        --begin;

    size_t end = cursor_pos;
    while(end < text.size() && isTokenChar(static_cast<unsigned char>(text[end])))
        ++end;

    // Also allow ':' as property separator (not part of long_name token chars for walk-back
    // except we treat text before ':' as component for scoped filter — include ':' in token).
    if(begin > 0 && text[begin - 1] == ':')
    {
        // extend left through component name before ':'
        size_t b2 = begin - 1;
        while(b2 > 0 && isTokenChar(static_cast<unsigned char>(text[b2 - 1])))
            --b2;
        begin = b2;
    }
    // If cursor sits just after ':', still grab left component + ':'
    if(cursor_pos > 0 && cursor_pos <= text.size() && text[cursor_pos - 1] == ':')
    {
        begin = cursor_pos - 1;
        while(begin > 0 && isTokenChar(static_cast<unsigned char>(text[begin - 1])))
            --begin;
        end = cursor_pos;
        while(end < text.size() && isTokenChar(static_cast<unsigned char>(text[end])))
            ++end;
    }

    span.begin = begin;
    span.end = end;
    span.text = text.substr(begin, end - begin);
    return span;
}

std::vector<std::string> rankChatNameCompletions(const std::string& prefix,
                                                 const std::vector<std::string>& dictionary,
                                                 size_t max_results)
{
    std::vector<std::string> prefix_hits;
    std::vector<std::string> contains_hits;
    for(const std::string& cand : dictionary)
    {
        if(cand.empty())
            continue;
        if(prefix.empty() || startsWithCi(cand, prefix))
            pushUnique(prefix_hits, cand);
        else if(containsCi(cand, prefix))
            pushUnique(contains_hits, cand);
    }
    std::vector<std::string> out = std::move(prefix_hits);
    for(const std::string& c : contains_hits)
        pushUnique(out, c);
    if(out.size() > max_results)
        out.resize(max_results);
    return out;
}

std::vector<std::string> filterCompletionsForScopedToken(
    const std::string& token,
    const std::vector<std::string>& long_names,
    const std::vector<std::string>& class_names,
    const std::vector<std::string>& property_names,
    size_t max_results)
{
    const size_t colon = token.find(':');
    const size_t last_dot = token.rfind('.');

    if(colon != std::string::npos)
    {
        const std::string prop_prefix =
            colon + 1 < token.size() ? token.substr(colon + 1) : std::string();
        std::vector<std::string> scoped;
        for(const std::string& p : property_names)
        {
            if(prop_prefix.empty() || startsWithCi(p, prop_prefix))
                pushUnique(scoped, p);
        }
        if(scoped.size() > max_results)
            scoped.resize(max_results);
        return scoped;
    }

    if(last_dot != std::string::npos)
    {
        const std::string parent = token.substr(0, last_dot);
        const std::string child_prefix =
            last_dot + 1 < token.size() ? token.substr(last_dot + 1) : std::string();
        std::vector<std::string> scoped;
        const std::string parent_dot = parent + ".";
        for(const std::string& ln : long_names)
        {
            if(!startsWithCi(ln, parent_dot))
                continue;
            // Suggest full long_name or remaining suffix
            if(child_prefix.empty() || startsWithCi(ln.substr(parent_dot.size()), child_prefix)
               || startsWithCi(ln, token))
                pushUnique(scoped, ln);
        }
        // Also properties when typing after a complete-looking path (heuristic: no further match)
        if(scoped.empty())
        {
            for(const std::string& p : property_names)
            {
                if(child_prefix.empty() || startsWithCi(p, child_prefix))
                    pushUnique(scoped, parent + "." + p);
            }
        }
        if(scoped.size() > max_results)
            scoped.resize(max_results);
        return scoped;
    }

    std::vector<std::string> dict = long_names;
    dict.insert(dict.end(), class_names.begin(), class_names.end());
    dict.insert(dict.end(), property_names.begin(), property_names.end());
    return rankChatNameCompletions(token, dict, max_results);
}

} // namespace RDK::LLM
