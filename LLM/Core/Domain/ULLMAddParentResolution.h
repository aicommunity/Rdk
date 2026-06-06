#ifndef RDK_ULLM_ADD_PARENT_RESOLUTION_H
#define RDK_ULLM_ADD_PARENT_RESOLUTION_H

#include "../LlmTypes.h"
#include "URdkDomainAccess.h"

#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

struct AddParentCandidate {
    std::string long_name;
    std::string class_name;
};

struct AddParentResolution {
    bool ok = false;
    bool needs_clarification = false;
    std::string parent_long_name;
    std::string message;
    std::vector<AddParentCandidate> candidates;
};

/// Validates or adjusts parent_long_name for add_component using pinned GUI scope.
AddParentResolution resolveValidAddParent(URdkDomainAccess& domain, const std::string& parent_hint,
                                          const std::string& class_name, int channel_index,
                                          const LLMGuiContextSnapshot& pin);

/// Legacy hook; returns canonical full parent path (identity). LLM add uses GetComponentL paths.
std::string engineContainerStringId(const std::string& parent_long_name,
                                  const LLMGuiContextSnapshot* gui);

} // namespace RDK::LLM

#endif
