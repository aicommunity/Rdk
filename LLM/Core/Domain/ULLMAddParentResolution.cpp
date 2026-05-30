#include "ULLMAddParentResolution.h"

#include "ULLMCurrentComponentScope.h"

namespace RDK::LLM {

namespace {

bool isRootModelParent(const std::string& parent)
{
    return parent.empty() || parent == "Model";
}

std::string preferredScopeParent(const LLMGuiContextSnapshot& pin)
{
    const std::string diagram = readDiagramScopeLongName(&pin);
    if(!diagram.empty() && !isRootModelParent(diagram))
        return diagram;
    const CurrentComponentScope cur = readCurrentComponentScope(&pin);
    if(cur.valid && !isRootModelParent(cur.long_name))
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

    const std::string prefix = scope.empty() || isRootModelParent(scope) ? std::string()
                                                                         : scope + ".";
    for(const auto& c : snap["components"])
    {
        const std::string ln = c.value("long_name", "");
        if(ln.empty())
            continue;
        if(!scope.empty() && !isRootModelParent(scope))
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

AddParentResolution resolveValidAddParent(URdkDomainAccess& domain, const std::string& parent_hint,
                                          const std::string& class_name, int channel_index,
                                          const LLMGuiContextSnapshot& pin)
{
    AddParentResolution res;
    (void)class_name;

    std::string parent = parent_hint;
    if(parent.empty())
        parent = preferredScopeParent(pin);

    if(isRootModelParent(parent))
    {
        const std::string scoped = preferredScopeParent(pin);
        if(!scoped.empty())
            parent = scoped;
    }

    if(!parent.empty() && !isRootModelParent(parent))
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

    if(isRootModelParent(parent))
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
    res.parent_long_name = parent.empty() ? std::string("Model") : parent;
    return res;
}

} // namespace RDK::LLM
