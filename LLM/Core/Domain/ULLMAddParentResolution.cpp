#include "ULLMAddParentResolution.h"

#include "ULLMCurrentComponentScope.h"

namespace RDK::LLM {

namespace {

std::string preferredScopeParent(const LLMGuiContextSnapshot& pin)
{
    const std::string diagram = readDiagramScopeLongName(&pin);
    if(!diagram.empty())
        return diagram;
    const CurrentComponentScope cur = readCurrentComponentScope(&pin);
    if(cur.valid && !isModelRootContainerToken(cur.long_name, &pin))
        return cur.long_name;
    return {};
}

} // namespace

std::string engineContainerStringId(const std::string& parent_long_name,
                                    const LLMGuiContextSnapshot* /*gui*/)
{
    return parent_long_name;
}

AddParentResolution resolveValidAddParent(URdkDomainAccess& domain, const std::string& parent_hint,
                                          const std::string& class_name, int channel_index,
                                          const LLMGuiContextSnapshot& pin)
{
    AddParentResolution res;
    (void)class_name;

    const std::string diagram_scope = readDiagramScopeLongName(&pin);
    const bool no_explicit_parent = parent_hint.empty() || parent_hint == "Model"
                                    || isModelRootContainerToken(parent_hint, &pin);

    if(no_explicit_parent)
    {
        std::string parent;
        if(!diagram_scope.empty())
            parent = diagram_scope;
        else
        {
            parent = preferredScopeParent(pin);
            if(parent.empty())
                parent = "Model";
        }
        res.ok = true;
        res.parent_long_name = parent;
        return res;
    }

    std::string resolved;
    if(domain.resolveComponentLongName(parent_hint, channel_index, resolved, diagram_scope).ok()
       || domain.resolveComponentLongName(parent_hint, channel_index, resolved, "").ok())
    {
        res.ok = true;
        res.parent_long_name = resolved;
        return res;
    }

    // Explicit unresolved hint: keep as-is (tool/engine or further clarify).
    res.ok = true;
    res.parent_long_name = parent_hint;
    return res;
}

} // namespace RDK::LLM
