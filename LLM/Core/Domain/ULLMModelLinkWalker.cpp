#include "ULLMModelLinkWalker.h"

#include "URdkDomainAccess.h"

#include "../../Core/Engine/UContainer.h"
#include "../../Core/Engine/UEnvironment.h"
#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UNet.h"

#include <cstdlib>
#include <unordered_set>

namespace RDK::LLM {

namespace {

int readPositiveEnv(const char* name, int default_value)
{
    const char* v = std::getenv(name);
    if(!v || !v[0])
        return default_value;
    try
    {
        const int parsed = std::stoi(v);
        return parsed > 0 ? parsed : default_value;
    }
    catch(...)
    {
        return default_value;
    }
}

void processQuad(const LinkQuad& quad, ModelLinkWalkResult& state, const ModelLinkWalkOptions& opts,
                 std::unordered_set<std::string>& seen_keys, bool& truncated_after_page)
{
    const std::string key = linkQuadDedupKey(quad);
    if(!seen_keys.insert(key).second)
        return;

    if(opts.match_quad && linkQuadEquals(quad, *opts.match_quad))
        state.found_match = true;

    const int index = static_cast<int>(seen_keys.size()) - 1;
    if(index < opts.offset)
        return;

    const int limit = opts.limit > 0 ? opts.limit : modelLinkWalkDefaultPageSize();
    if(static_cast<int>(state.links.size()) < limit)
        state.links.push_back(quad);
    else
        truncated_after_page = true;
}

void appendLinksFromContainerWalk(RDK::UContainer* walk_root, RDK::UContainer* model_root,
                                  ModelLinkWalkResult& state, const ModelLinkWalkOptions& opts,
                                  std::unordered_set<std::string>& seen_keys,
                                  bool& truncated_after_page)
{
    if(!walk_root || !model_root)
        return;

    RDK::UNet* net = dynamic_cast<RDK::UNet*>(walk_root);
    if(net)
    {
        RDK::UStringLinksList linkslist;
        net->GetLinks(linkslist, RDK::UEPtr<RDK::UContainer>(model_root), true,
                      RDK::UEPtr<RDK::UContainer>(walk_root));

        for(int i = 0; i < linkslist.GetSize(); ++i)
        {
            const RDK::UStringLink& link = linkslist[i];
            for(size_t j = 0; j < link.Connector.size(); ++j)
            {
                LinkQuad quad;
                quad.from_long_name = link.Item.Id;
                quad.from_property = link.Item.Name;
                quad.to_long_name = link.Connector[j].Id;
                quad.to_property = link.Connector[j].Name;
                if(quad.from_long_name.empty() || quad.to_long_name.empty())
                    continue;

                processQuad(quad, state, opts, seen_keys, truncated_after_page);
                if(opts.stop_on_first_match && state.found_match)
                    return;
            }
        }
    }

    if(opts.stop_on_first_match && state.found_match)
        return;

    const int n = walk_root->GetNumComponents();
    for(int i = 0; i < n; ++i)
    {
        RDK::UEPtr<RDK::UContainer> child = walk_root->GetComponentByIndex(i);
        if(!child)
            continue;
        appendLinksFromContainerWalk(child.Get(), model_root, state, opts, seen_keys,
                                     truncated_after_page);
        if(opts.stop_on_first_match && state.found_match)
            return;
    }
}

} // namespace

int modelLinkWalkDefaultPageSize()
{
    return readPositiveEnv("NMSDK_LLM_LIST_LINKS_PAGE_SIZE", 500);
}

int modelLinkSnapshotMaxLinks()
{
    return readPositiveEnv("NMSDK_LLM_SNAPSHOT_MAX_LINKS", 2000);
}

int modelLinkWalkMaxLimit()
{
    return readPositiveEnv("NMSDK_LLM_LIST_LINKS_MAX_LIMIT", 2000);
}

DomainStatus resolveModelLinkWalkRoot(URdkDomainAccess& domain, const ModelLinkWalkScope& scope,
                                      RDK::UContainer*& out_walk_root,
                                      RDK::UContainer*& out_model_root)
{
    out_walk_root = nullptr;
    out_model_root = nullptr;

    RDK::UEnvironment* env = domain.environment(scope.channel_index);
    if(!env)
        return {DomainStatusCode::NotInitialized, "Environment not available"};

    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};

    out_model_root = model.Get();
    out_walk_root = out_model_root;
    if(scope.root_long_name.empty())
        return {};

    RDK::UEPtr<RDK::UContainer> subtree = model->GetComponent(scope.root_long_name, true);
    if(!subtree)
        return {DomainStatusCode::ComponentNotFound,
                "Component not found: " + scope.root_long_name};
    out_walk_root = subtree.Get();
    return {};
}

std::vector<LinkQuad> dedupeModelLinkQuads(const std::vector<LinkQuad>& quads)
{
    std::vector<LinkQuad> out;
    std::unordered_set<std::string> seen;
    for(const LinkQuad& quad : quads)
    {
        if(seen.insert(linkQuadDedupKey(quad)).second)
            out.push_back(quad);
    }
    return out;
}

ModelLinkWalkResult applyLinkPage(const std::vector<LinkQuad>& all_quads,
                                  const ModelLinkWalkOptions& opts)
{
    ModelLinkWalkResult out;
    out.total_quads_seen = static_cast<int>(all_quads.size());

    if(opts.match_quad)
    {
        for(const LinkQuad& q : all_quads)
        {
            if(linkQuadEquals(q, *opts.match_quad))
            {
                out.found_match = true;
                if(opts.stop_on_first_match)
                    return out;
            }
        }
    }

    const int offset = std::max(0, opts.offset);
    const int limit = opts.limit > 0 ? opts.limit : modelLinkWalkDefaultPageSize();
    for(size_t i = static_cast<size_t>(offset);
        i < all_quads.size() && static_cast<int>(out.links.size()) < limit; ++i)
        out.links.push_back(all_quads[i]);

    if(static_cast<int>(all_quads.size()) > offset + static_cast<int>(out.links.size()))
        out.truncated = true;
    out.next_offset = offset + static_cast<int>(out.links.size());
    return out;
}

ModelLinkWalkResult walkModelLinks(RDK::UContainer* walk_root, RDK::UContainer* model_root,
                                   const ModelLinkWalkOptions& opts)
{
    ModelLinkWalkResult out;
    if(!walk_root || !model_root)
        return out;

    std::unordered_set<std::string> seen_keys;
    bool truncated_after_page = false;
    appendLinksFromContainerWalk(walk_root, model_root, out, opts, seen_keys, truncated_after_page);

    out.total_quads_seen = static_cast<int>(seen_keys.size());
    out.truncated = truncated_after_page && out.total_quads_seen > opts.offset + static_cast<int>(out.links.size());
    if(opts.limit > 0 && out.total_quads_seen > opts.offset + static_cast<int>(out.links.size()))
        out.truncated = true;
    out.next_offset = opts.offset + static_cast<int>(out.links.size());
    return out;
}

bool planSnapshotOrModelHasLink(const nlohmann::json& snapshot, URdkDomainAccess& domain,
                                const LinkQuad& quad, int channel_index)
{
    if(snapshotContainsLink(snapshot, quad))
        return true;
    if(!snapshot.value("links_truncated", false))
        return false;
    bool exists = false;
    if(domain.linkExistsInModel(quad, channel_index, "", exists).ok() && exists)
        return true;
    return false;
}

} // namespace RDK::LLM
