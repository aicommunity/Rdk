#ifndef RDK_ULLM_QUANTITY_PARSER_H
#define RDK_ULLM_QUANTITY_PARSER_H

#include <string>

namespace RDK::LLM {

struct ParsedQuantity {
    int count = 1;
    bool valid = false;
};

ParsedQuantity extractQuantity(const std::string& text);

} // namespace RDK::LLM

#endif
