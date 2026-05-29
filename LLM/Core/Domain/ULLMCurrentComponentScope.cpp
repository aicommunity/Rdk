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

} // namespace RDK::LLM
