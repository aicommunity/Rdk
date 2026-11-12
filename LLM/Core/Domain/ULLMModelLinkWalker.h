#ifndef RDK_ULLM_MODEL_LINK_WALKER_H
#define RDK_ULLM_MODEL_LINK_WALKER_H

#include "ULLMLinkIdentity.h"

#include "../LlmTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace RDK {
class UContainer;
}

namespace RDK::LLM {

class URdkDomainAccess;

struct ModelLinkWalkScope {
    int channel_index = 0;
    std::string root_long_name;
};

struct ModelLinkWalkOptions {
    int offset = 0;
    int limit = 500;
    bool count_all = false;
    bool stop_on_first_match = false;
    std::optional<LinkQuad> match_quad;
    /// When set, only matching quads count toward offset/limit / result page.
    std::optional<ModelLinkListFilters> subtree_filters;
};

struct ModelLinkWalkResult {
    std::vector<LinkQuad> links;
    int total_quads_seen = 0;
    int total_matching = 0;
    bool truncated = false;
    int next_offset = 0;
    bool found_match = false;
};

int modelLinkWalkDefaultPageSize();
int modelLinkSnapshotMaxLinks();
int modelLinkWalkMaxLimit();

DomainStatus resolveModelLinkWalkRoot(URdkDomainAccess& domain, const ModelLinkWalkScope& scope,
                                      RDK::UContainer*& out_walk_root,
                                      RDK::UContainer*& out_model_root);

ModelLinkWalkResult walkModelLinks(RDK::UContainer* walk_root, RDK::UContainer* model_root,
                                   const ModelLinkWalkOptions& opts);

/// Testable pagination over a pre-collected deduped list.
ModelLinkWalkResult applyLinkPage(const std::vector<LinkQuad>& all_quads,
                                  const ModelLinkWalkOptions& opts);

/// Collapse exact quad duplicates (same dedup key as walkModelLinks).
std::vector<LinkQuad> dedupeModelLinkQuads(const std::vector<LinkQuad>& quads);

bool planSnapshotOrModelHasLink(const nlohmann::json& snapshot, URdkDomainAccess& domain,
                                const LinkQuad& quad, int channel_index);

} // namespace RDK::LLM

#endif
