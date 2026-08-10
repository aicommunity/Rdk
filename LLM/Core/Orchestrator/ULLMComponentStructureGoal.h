#ifndef RDK_ULLM_COMPONENT_STRUCTURE_GOAL_H
#define RDK_ULLM_COMPONENT_STRUCTURE_GOAL_H

#include <string>
#include <vector>

namespace RDK::LLM {

struct ParsedDendriteStructureGoal {
    bool ok = false;
    std::string component_token;
    int dendrite_count = 0;
    std::vector<int> lengths;
};

/// Structure-mutation goals (dendrites/soma/membrane parts) — need docs/ClDesc, not add_component.
bool isComponentStructureGoal(const std::string& text);

std::string componentStructureEphemeralHint();

/// Parse «N дендрит(а) … длины a, b, c» + optional component token (e.g. PNeuron).
ParsedDendriteStructureGoal parseDendriteStructureGoal(const std::string& text);

} // namespace RDK::LLM

#endif
