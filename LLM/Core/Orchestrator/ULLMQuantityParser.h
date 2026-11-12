#ifndef RDK_ULLM_QUANTITY_PARSER_H
#define RDK_ULLM_QUANTITY_PARSER_H

#include <string>

namespace RDK::LLM {

enum class QuantityHeuristicSource {
    None,
    NumericSuffix,
    NumericBare,
    Word,
};

struct ParsedQuantity {
    int count = 1;
    bool valid = false;
    QuantityHeuristicSource source = QuantityHeuristicSource::None;
};

/// Local heuristic extraction (digits, number words). Does not call LLM.
ParsedQuantity extractQuantityHeuristic(const std::string& text);

/// @deprecated Prefer resolveUserQuantity; kept for tests and internal merge.
inline ParsedQuantity extractQuantity(const std::string& text)
{
    return extractQuantityHeuristic(text);
}

/// True when text likely mentions an operation count (mutate/add/remove/neuron cues).
bool looksLikeQuantityCue(const std::string& text);

} // namespace RDK::LLM

#endif
