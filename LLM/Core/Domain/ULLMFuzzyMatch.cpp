#include "ULLMFuzzyMatch.h"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace RDK::LLM {

namespace {

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

} // namespace

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

double fuzzyScore(const std::string& query_lower, const std::string& candidate_lower)
{
    const int dist = levenshteinDistance(query_lower, candidate_lower);
    const int maxlen =
        static_cast<int>(std::max(query_lower.size(), candidate_lower.size()));
    const double norm =
        maxlen > 0 ? (static_cast<double>(dist) / static_cast<double>(maxlen)) : 1.0;
    double score = 1.0 - norm;
    if(!query_lower.empty() && candidate_lower.find(query_lower) != std::string::npos)
        score += 0.15;
    if(!query_lower.empty() && candidate_lower.rfind(query_lower, 0) == 0)
        score += 0.1;
    return std::min(1.0, score);
}

std::vector<FuzzyHit> fuzzyRank(const std::string& query,
                                const std::vector<std::string>& candidates,
                                size_t max_hits,
                                double min_score)
{
    std::vector<FuzzyHit> out;
    if(query.empty() || candidates.empty())
        return out;

    const std::string q = toLowerAscii(query);
    out.reserve(std::min(max_hits, candidates.size()));
    for(const std::string& c : candidates)
    {
        const double score = fuzzyScore(q, toLowerAscii(c));
        if(score < min_score)
            continue;
        out.push_back({c, score});
    }

    std::sort(out.begin(), out.end(), [](const FuzzyHit& a, const FuzzyHit& b) {
        if(std::fabs(a.score - b.score) > 1e-9)
            return a.score > b.score;
        return a.name < b.name;
    });
    if(out.size() > max_hits)
        out.resize(max_hits);
    return out;
}

std::string bestFuzzyWithinDistance(const std::string& query,
                                    const std::vector<std::string>& candidates,
                                    int max_edit_distance)
{
    if(query.empty() || candidates.empty())
        return {};

    const std::string qlower = toLowerAscii(query);
    std::string best;
    int best_dist = max_edit_distance + 1;
    for(const std::string& candidate : candidates)
    {
        const int d = levenshteinDistance(qlower, toLowerAscii(candidate));
        if(d < best_dist)
        {
            best_dist = d;
            best = candidate;
        }
    }
    if(best_dist <= max_edit_distance)
        return best;
    return {};
}

} // namespace RDK::LLM
