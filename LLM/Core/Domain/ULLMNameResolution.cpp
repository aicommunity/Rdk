#include "ULLMNameResolution.h"

#include "../Orchestrator/ULLMLibraryScopeHint.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

namespace RDK::LLM {

namespace {

std::string trimCopy(const std::string& s)
{
    size_t b = 0;
    while(b < s.size() && std::isspace(static_cast<unsigned char>(s[b])))
        ++b;
    size_t e = s.size();
    while(e > b && std::isspace(static_cast<unsigned char>(s[e - 1])))
        --e;
    return s.substr(b, e - b);
}

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool isUnsignedListIndex(const std::string& s)
{
    if(s.empty())
        return false;
    for(char c : s)
    {
        if(!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

int levenshteinDistance(const std::string& a, const std::string& b)
{
    const size_t n = a.size();
    const size_t m = b.size();
    if(n == 0)
        return static_cast<int>(m);
    if(m == 0)
        return static_cast<int>(n);
    std::vector<int> prev(m + 1);
    std::vector<int> cur(m + 1);
    for(size_t j = 0; j <= m; ++j)
        prev[j] = static_cast<int>(j);
    for(size_t i = 1; i <= n; ++i)
    {
        cur[0] = static_cast<int>(i);
        for(size_t j = 1; j <= m; ++j)
        {
            const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            cur[j] = std::min({cur[j - 1] + 1, prev[j] + 1, prev[j - 1] + cost});
        }
        prev.swap(cur);
    }
    return prev[m];
}

struct ClassCandidate {
    std::string class_name;
    double score = 0.0;
};

bool looksLikeClassIdentifierLocal(const std::string& token)
{
    if(token.empty())
        return false;
    const unsigned char first = static_cast<unsigned char>(token[0]);
    if(!std::isalpha(first))
        return false;
    for(char c : token)
    {
        if(std::isalnum(static_cast<unsigned char>(c)) || c == '_')
            continue;
        return false;
    }
    return true;
}

std::vector<ClassCandidate> findSimilarRegisteredClasses(const std::string& query,
                                                         const std::vector<std::string>& registered,
                                                         size_t max_candidates = 8)
{
    std::vector<ClassCandidate> out;
    if(query.empty() || registered.empty())
        return out;

    const std::string q = toLowerAscii(query);
    out.reserve(std::min(max_candidates, registered.size()));
    for(const std::string& c : registered)
    {
        const std::string cl = toLowerAscii(c);
        const int dist = levenshteinDistance(q, cl);
        const int maxlen = static_cast<int>(std::max(q.size(), cl.size()));
        const double norm = maxlen > 0 ? (static_cast<double>(dist) / static_cast<double>(maxlen)) : 1.0;
        double score = 1.0 - norm;
        if(!q.empty() && cl.find(q) != std::string::npos)
            score += 0.15;
        if(!q.empty() && cl.rfind(q, 0) == 0)
            score += 0.1;
        score = std::min(1.0, score);
        if(score < 0.35)
            continue;
        out.push_back({c, score});
    }

    std::sort(out.begin(), out.end(), [](const ClassCandidate& a, const ClassCandidate& b) {
        if(std::fabs(a.score - b.score) > 1e-9)
            return a.score > b.score;
        return a.class_name < b.class_name;
    });
    if(out.size() > max_candidates)
        out.resize(max_candidates);
    return out;
}

std::string extractClassNameTokenFromUserText(const std::string& user_text)
{
    const std::string trimmed = trimCopy(user_text);
    if(trimmed.empty())
        return trimmed;
    if(trimmed.find_first_of(" \t\n\r") == std::string::npos)
        return trimmed;

    const size_t last_space = trimmed.find_last_of(" \t\n\r");
    if(last_space != std::string::npos && last_space + 1 < trimmed.size())
    {
        const std::string last = trimCopy(trimmed.substr(last_space + 1));
        if(looksLikeClassIdentifierLocal(last))
            return last;
    }

    const std::string lower = toLowerAscii(trimmed);
    static const char* kKeywords[] = {"нейрон",
                                      "neuron",
                                      "синапс",
                                      "synapse",
                                      "membrane",
                                      "мембран",
                                      "manipulator",
                                      "манипулятор",
                                      nullptr};
    for(const char** kw = kKeywords; *kw; ++kw)
    {
        if(lower == *kw)
            return *kw;
    }

    if(last_space != std::string::npos && last_space + 1 < trimmed.size())
        return trimCopy(trimmed.substr(last_space + 1));
    return trimmed;
}

} // namespace

bool isRegisteredClassName(const std::vector<std::string>& registered, const std::string& name)
{
    if(name.empty())
        return false;
    const std::string lower = toLowerAscii(name);
    for(const std::string& c : registered)
    {
        if(toLowerAscii(c) == lower)
            return true;
    }
    return false;
}

std::string canonicalRegisteredClassName(const std::vector<std::string>& registered,
                                         const std::string& name)
{
    const std::string lower = toLowerAscii(name);
    for(const std::string& c : registered)
    {
        if(toLowerAscii(c) == lower)
            return c;
    }
    return name;
}

bool looksLikeClassIdentifier(const std::string& token)
{
    return looksLikeClassIdentifierLocal(token);
}

std::string extractClassNameQuery(const std::string& class_name_field, const std::string& user_text)
{
    const std::string from_user = extractClassNameTokenFromUserText(user_text);
    if(!from_user.empty())
        return from_user;
    return trimCopy(class_name_field);
}

RegisteredClassResolution resolveRegisteredClassName(const std::string& query,
                                                   const std::vector<std::string>& registered)
{
    RegisteredClassResolution result;
    if(query.empty() || registered.empty())
        return result;

    for(const std::string& c : registered)
    {
        if(c == query)
        {
            result.status = RegisteredClassResolution::Status::Resolved;
            result.class_name = c;
            return result;
        }
    }

    const std::string q_lower = toLowerAscii(query);
    std::vector<std::string> case_insensitive;
    case_insensitive.reserve(registered.size());
    for(const std::string& c : registered)
    {
        if(toLowerAscii(c) == q_lower)
            case_insensitive.push_back(c);
    }
    if(case_insensitive.size() == 1)
    {
        result.status = RegisteredClassResolution::Status::Resolved;
        result.class_name = case_insensitive.front();
        return result;
    }
    if(case_insensitive.size() > 1)
    {
        result.status = RegisteredClassResolution::Status::Ambiguous;
        for(const std::string& c : case_insensitive)
            result.candidates.push_back({c, 1.0});
        return result;
    }

    const std::string aliased = resolveKnownClassAlias(query);
    if(aliased != query)
        return resolveRegisteredClassName(aliased, registered);

    const std::vector<ClassCandidate> fuzzy = findSimilarRegisteredClasses(query, registered, 8);
    if(fuzzy.empty())
        return result;

    if(fuzzy.size() == 1)
    {
        result.status = RegisteredClassResolution::Status::Resolved;
        result.class_name = fuzzy.front().class_name;
        return result;
    }

    result.status = RegisteredClassResolution::Status::Ambiguous;
    for(const ClassCandidate& c : fuzzy)
        result.candidates.push_back({c.class_name, c.score});
    return result;
}

ComponentEntityResolution resolveComponentEntity(
    const std::string& query, const nlohmann::json& snapshot_components,
    const std::optional<std::string>& class_filter)
{
    ComponentEntityResolution result;
    const std::string trimmed = trimCopy(query);
    if(trimmed.empty() || !snapshot_components.is_array())
        return result;

    const std::string qlower = toLowerAscii(trimmed);
    std::vector<std::pair<std::string, double>> tier2_matches;

    for(const nlohmann::json& c : snapshot_components)
    {
        if(!c.is_object())
            continue;
        const std::string sn = c.value("short_name", "");
        const std::string ln = c.value("long_name", "");
        const std::string cn = c.value("class_name", "");
        if(class_filter && cn != *class_filter)
            continue;

        const std::string sn_lower = toLowerAscii(sn);
        const std::string ln_lower = toLowerAscii(ln);

        if(sn == trimmed || ln == trimmed)
        {
            result.status = ComponentEntityResolution::Status::Resolved;
            result.canonical_long_name = ln;
            return result;
        }
        if(sn_lower == qlower || ln_lower == qlower)
        {
            tier2_matches.push_back({ln, 0.98});
            continue;
        }

        double score = 0.0;
        if(!qlower.empty() && ln_lower.size() >= qlower.size()
           && ln_lower.compare(ln_lower.size() - qlower.size(), qlower.size(), qlower) == 0)
            score = 0.90;
        else if(sn_lower.find(qlower) != std::string::npos || ln_lower.find(qlower) != std::string::npos)
            score = 0.65;

        if(score > 0.0)
            result.candidates.push_back({ln, score});
    }

    if(tier2_matches.size() == 1)
    {
        result.status = ComponentEntityResolution::Status::Resolved;
        result.canonical_long_name = tier2_matches.front().first;
        return result;
    }
    if(tier2_matches.size() > 1)
    {
        result.status = ComponentEntityResolution::Status::Ambiguous;
        result.candidates = std::move(tier2_matches);
        return result;
    }

    std::sort(result.candidates.begin(), result.candidates.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    if(result.candidates.empty())
        return result;

    if(result.candidates.size() == 1)
    {
        result.status = ComponentEntityResolution::Status::Resolved;
        result.canonical_long_name = result.candidates.front().first;
        return result;
    }

    const double top = result.candidates[0].second;
    const double second = result.candidates[1].second;
    if(top >= 0.90 && (top - second) >= 0.15)
    {
        result.status = ComponentEntityResolution::Status::Resolved;
        result.canonical_long_name = result.candidates.front().first;
        return result;
    }

    result.status = ComponentEntityResolution::Status::Ambiguous;
    return result;
}

std::optional<std::string> pickFromNumberedList(const std::string& user_text,
                                                const nlohmann::json& candidates,
                                                const char* value_key)
{
    const std::string trimmed = trimCopy(user_text);
    if(trimmed.empty() || !candidates.is_array() || candidates.empty() || !value_key)
        return std::nullopt;

    if(isUnsignedListIndex(trimmed))
    {
        const unsigned long idx = std::stoul(trimmed);
        if(idx >= 1 && idx <= candidates.size())
        {
            const nlohmann::json& entry = candidates[idx - 1];
            if(entry.is_object() && entry.contains(value_key) && entry[value_key].is_string())
                return entry[value_key].get<std::string>();
        }
        return std::nullopt;
    }

    const std::string lower = toLowerAscii(trimmed);
    for(const nlohmann::json& entry : candidates)
    {
        if(!entry.is_object() || !entry.contains(value_key) || !entry[value_key].is_string())
            continue;
        const std::string value = entry[value_key].get<std::string>();
        if(value == trimmed || toLowerAscii(value) == lower)
            return value;
    }
    return std::nullopt;
}

} // namespace RDK::LLM
