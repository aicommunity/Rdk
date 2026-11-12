#include "ULLMNameResolution.h"

#include "ULLMFuzzyMatch.h"
#include "../Orchestrator/ULLMLibraryScopeHint.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

namespace RDK::LLM {

namespace {

std::string stripTrailingPunctuation(std::string token)
{
    while(!token.empty())
    {
        const char c = token.back();
        if(c == '.' || c == ',' || c == ';' || c == ':' || c == '!' || c == '?')
            token.pop_back();
        else
            break;
    }
    return token;
}

std::vector<std::string> tokenizeUserText(const std::string& user_text)
{
    std::vector<std::string> tokens;
    std::string current;
    for(char c : user_text)
    {
        if(std::isspace(static_cast<unsigned char>(c)))
        {
            if(!current.empty())
            {
                tokens.push_back(stripTrailingPunctuation(current));
                current.clear();
            }
            continue;
        }
        current += c;
    }
    if(!current.empty())
        tokens.push_back(stripTrailingPunctuation(current));
    return tokens;
}

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
    const std::vector<FuzzyHit> ranked = fuzzyRank(query, registered, max_candidates, 0.35);
    std::vector<ClassCandidate> out;
    out.reserve(ranked.size());
    for(const FuzzyHit& h : ranked)
        out.push_back({h.name, h.score});
    return out;
}

} // namespace

std::optional<std::string> findExplicitRegisteredClassInUserText(
    const std::string& user_text, const std::vector<std::string>& registered)
{
    if(user_text.empty() || registered.empty())
        return std::nullopt;

    std::optional<std::string> best;
    size_t best_score = 0;
    size_t token_index = 0;
    for(const std::string& raw_token : tokenizeUserText(user_text))
    {
        ++token_index;
        if(!looksLikeClassIdentifierLocal(raw_token))
            continue;
        if(!isRegisteredClassName(registered, raw_token))
            continue;

        const std::string canonical = canonicalRegisteredClassName(registered, raw_token);
        size_t score = canonical.size() * 4 + token_index;
        if(canonical == raw_token)
            score += 32;
        if(!best || score >= best_score)
        {
            best = canonical;
            best_score = score;
        }
    }
    return best;
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
        if(looksLikeClassIdentifier(last))
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

RegisteredClassResolution resolvePropertyNameFromCatalog(const std::string& query,
                                                         const std::vector<std::string>& catalog)
{
    RegisteredClassResolution result;
    const std::string trimmed = trimCopy(query);
    if(trimmed.empty() || catalog.empty())
        return result;

    for(const std::string& name : catalog)
    {
        if(name == trimmed)
        {
            result.status = RegisteredClassResolution::Status::Resolved;
            result.class_name = name;
            return result;
        }
    }

    const std::string qlower = toLowerAscii(trimmed);
    std::vector<std::string> case_insensitive;
    for(const std::string& name : catalog)
    {
        if(toLowerAscii(name) == qlower)
            case_insensitive.push_back(name);
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

    // Strip underscores / common typo noise for soft match (numDendridet… → NumDendrite…)
    auto stripNoise = [](std::string s) {
        s = toLowerAscii(std::move(s));
        s.erase(std::remove(s.begin(), s.end(), '_'), s.end());
        return s;
    };
    const std::string qstripped = stripNoise(trimmed);
    std::vector<std::string> stripped_hits;
    for(const std::string& name : catalog)
    {
        if(stripNoise(name) == qstripped)
            stripped_hits.push_back(name);
    }
    if(stripped_hits.size() == 1)
    {
        result.status = RegisteredClassResolution::Status::Resolved;
        result.class_name = stripped_hits.front();
        return result;
    }

    const std::vector<ClassCandidate> fuzzy = findSimilarRegisteredClasses(trimmed, catalog, 8);
    if(fuzzy.empty())
        return result;
    if(fuzzy.size() == 1 || (fuzzy.size() >= 2 && fuzzy[0].score - fuzzy[1].score > 0.08))
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
        else
        {
            // Edit-distance on short_name / last path segment (shared ULLMFuzzyMatch).
            const double sn_score = fuzzyScore(qlower, sn_lower);
            std::string leaf = sn_lower;
            const size_t dot = ln_lower.find_last_of('.');
            if(dot != std::string::npos && dot + 1 < ln_lower.size())
                leaf = ln_lower.substr(dot + 1);
            const double leaf_score = fuzzyScore(qlower, leaf);
            const double edit = std::max(sn_score, leaf_score);
            if(edit >= 0.55)
                score = 0.50 + 0.35 * edit;
        }

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
