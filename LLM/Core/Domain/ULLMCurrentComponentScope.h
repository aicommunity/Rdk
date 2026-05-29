#ifndef RDK_ULLM_CURRENT_COMPONENT_SCOPE_H
#define RDK_ULLM_CURRENT_COMPONENT_SCOPE_H

#include <string>

namespace RDK::LLM {

struct LLMGuiContextSnapshot;

struct CurrentComponentScope {
    std::string long_name;
    std::string id;
    bool valid = false;
};

/// Kernel CurrentComponent (authoritative); optional GUI snapshot fallback.
CurrentComponentScope readCurrentComponentScope(const LLMGuiContextSnapshot* gui_fallback = nullptr);

/// Open schematic drill level; empty at root.
std::string readDiagramScopeLongName(const LLMGuiContextSnapshot* gui);

} // namespace RDK::LLM

#endif
