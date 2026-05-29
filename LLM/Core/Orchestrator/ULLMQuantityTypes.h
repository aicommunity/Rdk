#ifndef RDK_ULLM_QUANTITY_TYPES_H
#define RDK_ULLM_QUANTITY_TYPES_H

#include <string>

namespace RDK::LLM {

enum class QuantitySource {
    None,
    Heuristic,
    Llm,
};

struct ResolvedUserQuantity {
    int primary = 1;
    bool valid = false;
    QuantitySource source = QuantitySource::None;
    std::string bound_turn_hash;
};

} // namespace RDK::LLM

#endif
