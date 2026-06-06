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

std::vector<AddParentCandidate> listContainerCandidatesUnderScope(URdkDomainAccess& domain,
                                                                  int channel_index,
                                                                  const std::string& scope)
{
    std::vector<AddParentCandidate> out;
    nlohmann::json snap;
    const DomainStatus st = domain.listNetSnapshot(snap, channel_index, 5000, scope);
    if(!st.ok() || !snap.contains("components"))
        return out;

    const std::string prefix =
        scope.empty() || isModelRootContainerToken(scope, nullptr) ? std::string() : scope + ".";
    for(const auto& c : snap["components"])
    {
        const std::string ln = c.value("long_name", "");
        if(ln.empty())
            continue;
        if(!scope.empty() && !isModelRootContainerToken(scope, nullptr))
        {
            if(ln != scope && ln.rfind(prefix, 0) != 0)
                continue;
        }
        const std::string cls = c.value("class_name", "");
        if(cls.empty())
            continue;
        out.push_back({ln, cls});
    }
    return out;
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

    std::string parent = parent_hint;
    const std::string diagram_scope = readDiagramScopeLongName(&pin);
    if(!diagram_scope.empty() && (parent.empty() || parent == "Model"))
        parent = diagram_scope;
    else if(parent.empty() || isModelRootContainerToken(parent, &pin))
        parent = preferredScopeParent(pin);

    if(!parent.empty() && !isModelRootContainerToken(parent, &pin))
    {
        std::string resolved;
        const std::string scope = readDiagramScopeLongName(&pin);
        if(domain.resolveComponentLongName(parent, channel_index, resolved, scope).ok()
           || domain.resolveComponentLongName(parent, channel_index, resolved, "").ok())
        {
            res.ok = true;
            res.parent_long_name = resolved;
            return res;
        }
    }

    if(parent.empty() || isModelRootContainerToken(parent, &pin))
    {
        const std::string scope = readDiagramScopeLongName(&pin);
        const std::vector<AddParentCandidate> candidates =
            listContainerCandidatesUnderScope(domain, channel_index, scope);
        if(candidates.size() == 1)
        {
            res.ok = true;
            res.parent_long_name = candidates.front().long_name;
            return res;
        }
        if(candidates.size() > 1)
        {
            res.needs_clarification = true;
            res.message = "Which container should the new component be added under?";
            res.candidates = candidates;
            return res;
        }
    }

    res.ok = true;
    res.parent_long_name = parent;
    return res;
}

} // namespace RDK::LLM
