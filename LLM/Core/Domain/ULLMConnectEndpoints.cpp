#include "ULLMConnectEndpoints.h"

#include "../Orchestrator/ULLMConnectPlanParsing.h"
#include "../Session/ULLMSessionGraphMemory.h"
#include "URdkDomainAccess.h"

#include <algorithm>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::unordered_set<std::string> collectLinkedEndpointLongNames(URdkDomainAccess& domain,
                                                              const ModelLinkWalkScope& scope,
                                                              bool& incomplete)
{
    incomplete = false;
    std::unordered_set<std::string> linked;
    RDK::UContainer* walk_root = nullptr;
    RDK::UContainer* model_root = nullptr;
    if(!resolveModelLinkWalkRoot(domain, scope, walk_root, model_root).ok())
    {
        incomplete = true;
        return linked;
    }

    int offset = 0;
  const int page = modelLinkWalkDefaultPageSize();
    for(;;)
    {
        ModelLinkWalkOptions opts;
        opts.offset = offset;
        opts.limit = page;
        opts.count_all = true;
        const ModelLinkWalkResult walk = walkModelLinks(walk_root, model_root, opts);
        for(const LinkQuad& q : walk.links)
        {
            linked.insert(q.from_long_name);
            linked.insert(q.to_long_name);
        }
        if(!walk.truncated)
            break;
        offset = walk.next_offset;
        if(offset <= 0 || walk.links.empty())
        {
            incomplete = true;
            break;
        }
    }
    return linked;
}

} // namespace

std::vector<std::string> collectRemainingEndpoints(URdkDomainAccess& domain,
                                                   const nlohmann::json& snapshot_components,
                                                   const ParsedConnectGoal& parsed,
                                                   const ConversationState* session,
                                                   int channel_index,
                                                   bool& links_incomplete_out)
{
    links_incomplete_out = false;
    if(parsed.remaining_scope == ConnectRemainingScope::SessionDelta)
    {
        if(!session)
            return {};
        return sessionRemainingLongNames(*session);
    }

    ModelLinkWalkScope scope;
    scope.channel_index = channel_index;
    bool incomplete = false;
    const std::unordered_set<std::string> linked =
        collectLinkedEndpointLongNames(domain, scope, incomplete);
    if(incomplete)
    {
        links_incomplete_out = true;
        return {};
    }

    std::vector<std::string> out;
    for(const auto& comp : snapshot_components)
    {
        const std::string ln = comp.value("long_name", "");
        if(ln.empty())
            continue;
        if(linked.count(ln) == 0)
            out.push_back(ln);
    }

    std::sort(out.begin(), out.end(), [&snapshot_components](const std::string& a, const std::string& b) {
        auto short_of = [&](const std::string& ln) {
            for(const auto& c : snapshot_components)
                if(c.value("long_name", "") == ln)
                    return c.value("short_name", ln);
            return ln;
        };
        std::string sa = short_of(a);
        std::string sb = short_of(b);
        std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
        std::transform(sb.begin(), sb.end(), sb.begin(), ::tolower);
        if(sa != sb)
            return sa < sb;
        return a < b;
    });
    return out;
}

} // namespace RDK::LLM
