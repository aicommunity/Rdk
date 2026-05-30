#include "ULLMCurrentComponentScope.h"

#include "../LlmTypes.h"

namespace RDK::LLM {

CurrentComponentScope readCurrentComponentScope(const LLMGuiContextSnapshot* gui_fallback)
{
    CurrentComponentScope scope;
    if(!gui_fallback)
        return scope;

    if(!gui_fallback->current_component_long_name.empty())
    {
        scope.long_name = gui_fallback->current_component_long_name;
        scope.valid = true;
    }
    if(!gui_fallback->current_component_id.empty())
        scope.id = gui_fallback->current_component_id;

    if(!scope.valid && !gui_fallback->focused_component_long_name.empty())
    {
        scope.long_name = gui_fallback->focused_component_long_name;
        scope.valid = true;
    }
    return scope;
}

std::string readDiagramScopeLongName(const LLMGuiContextSnapshot* gui)
{
    if(!gui)
        return {};
    return gui->diagram_scope_long_name;
}

bool isAtRootDiagramView(const LLMGuiContextSnapshot* gui)
{
    return readDiagramScopeLongName(gui).empty();
}

bool isModelRootContainerToken(const std::string& parent_long_name,
                                const LLMGuiContextSnapshot* gui)
{
    if(parent_long_name != "Model")
        return false;
    return isAtRootDiagramView(gui);
}

} // namespace RDK::LLM
