#ifndef RDK_ULLM_FUZZY_MATCH_H
#define RDK_ULLM_FUZZY_MATCH_H

#include <string>
#include <vector>

namespace RDK::LLM {

struct FuzzyHit {
    std::string name;
    double score = 0.0;
};

/// Classic Levenshtein edit distance (shared by class/property/component/library resolve).
int levenshteinDistance(const std::string& a, const std::string& b);

/// Score in [0,1]: 1 - dist/maxlen, with substring (+0.15) and prefix (+0.1) bonuses.
double fuzzyScore(const std::string& query_lower, const std::string& candidate_lower);

/// Rank candidates by fuzzyScore(query, candidate); drop below min_score; cap at max_hits.
std::vector<FuzzyHit> fuzzyRank(const std::string& query,
                                const std::vector<std::string>& candidates,
                                size_t max_hits = 8,
                                double min_score = 0.35);

/// Best candidate within max_edit_distance (case-insensitive). Empty if none.
std::string bestFuzzyWithinDistance(const std::string& query,
                                    const std::vector<std::string>& candidates,
                                    int max_edit_distance = 3);

} // namespace RDK::LLM

#endif
