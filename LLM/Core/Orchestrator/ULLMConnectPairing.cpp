#include "ULLMConnectPairing.h"

#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "../Context/ULinkPatternCatalog.h"
#include "../Domain/ULLMConnectPortInference.h"
#include "../Domain/URdkDomainAccess.h"
#include <algorithm>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::optional<std::string> classNameFromSnapshot(const nlohmann::json& components,
                                                 const std::string& long_name)
{
    for(const auto& c : components)
    {
        if(c.value("long_name", "") == long_name)
            return c.value("class_name", "");
    }
    return std::nullopt;
}

double pairScore(URdkDomainAccess& domain, ULinkPatternCatalog& catalog,
                 const ULLMConnectSemanticsCatalog* semantics, int channel_index,
                 const std::string& from_ln, const std::string& to_ln,
                 const nlohmann::json* snapshot_components)
{
    std::string from_class;
    std::string to_class;
    if(!domain.getComponentClassName(from_ln, channel_index, from_class).ok())
    {
        if(snapshot_components)
            from_class = classNameFromSnapshot(*snapshot_components, from_ln).value_or("");
        if(from_class.empty())
            return 0.0;
    }
    if(!domain.getComponentClassName(to_ln, channel_index, to_class).ok())
    {
        if(snapshot_components)
            to_class = classNameFromSnapshot(*snapshot_components, to_ln).value_or("");
        if(to_class.empty())
            return 0.0;
    }
    if(semantics && !semantics->empty())
    {
        const auto s = semantics->suggestContainerPair(from_class, to_class, 1);
        if(!s.empty())
            return s[0].score;
    }
    const auto c = catalog.suggest(from_class, to_class, 1);
    if(!c.empty())
        return c[0].score;
    return 0.0;
}

void fillPorts(PairingCandidate& p, URdkDomainAccess& domain, ULinkPatternCatalog& catalog,
               const ULLMConnectSemanticsCatalog* semantics, int channel_index,
               const std::string& goal_en, bool prefer_internal_semantics)
{
    nlohmann::json args = {{"from_long_name", p.from_long_name},
                           {"to_long_name", p.to_long_name},
                           {"from_property", p.from_property},
                           {"to_property", p.to_property},
                           {"channel_index", channel_index}};
    ConnectPortInferenceResult inf = inferConnectPorts(
        args, domain, catalog, channel_index, semantics, goal_en, prefer_internal_semantics);
    if(inf.ok)
    {
        p.from_property = args.value("from_property", "");
        p.to_property = args.value("to_property", "");
    }
}

std::optional<std::string> pickAutoHub(const PairingRequest& req, PairingResult& issues_out)
{
    std::string best;
    double best_score = -1.0;
    int ties = 0;
    for(const std::string& hub : req.endpoints)
    {
        int compatible = 0;
        double score_sum = 0.0;
        for(const std::string& other : req.endpoints)
        {
            if(other == hub)
                continue;
            const double sc =
                pairScore(*req.domain, *req.catalog, req.semantics, req.channel_index, hub,
                          other, req.snapshot_components);
            if(sc >= 0.6)
            {
                ++compatible;
                score_sum += sc;
            }
        }
        const double hub_score = compatible > 0 ? score_sum : 0.0;
        if(hub_score > best_score)
        {
            best_score = hub_score;
            best = hub;
            ties = 1;
        }
        else if(hub_score == best_score && hub_score > 0.0)
        {
            ++ties;
        }
    }
    if(ties > 1)
    {
        issues_out.issues.push_back("ambiguous_hub");
        return std::nullopt;
    }
    if(best.empty())
        return std::nullopt;
    return best;
}

std::vector<std::pair<std::string, std::string>> buildGreedySequentialPairs(
    const PairingRequest& req, PairingResult& out)
{
    struct ScoredPair {
        std::string from;
        std::string to;
        double score = 0.0;
    };
    std::vector<ScoredPair> candidates;
    for(size_t i = 0; i < req.endpoints.size(); ++i)
    {
        for(size_t j = i + 1; j < req.endpoints.size(); ++j)
        {
            const double sc =
                pairScore(*req.domain, *req.catalog, req.semantics, req.channel_index,
                          req.endpoints[i], req.endpoints[j], req.snapshot_components);
            candidates.push_back({req.endpoints[i], req.endpoints[j], sc});
        }
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const ScoredPair& a, const ScoredPair& b) { return a.score > b.score; });

    std::unordered_set<std::string> used;
    std::vector<std::pair<std::string, std::string>> raw_pairs;
    const int want = req.link_count > 0 ? req.link_count : static_cast<int>(candidates.size());
    for(const ScoredPair& c : candidates)
    {
        if(static_cast<int>(raw_pairs.size()) >= want)
            break;
        if(used.count(c.from) || used.count(c.to))
            continue;
        if(c.score < 0.6)
            continue;
        used.insert(c.from);
        used.insert(c.to);
        raw_pairs.push_back({c.from, c.to});
    }
    if(req.link_count > 0 && static_cast<int>(raw_pairs.size()) < req.link_count)
        out.issues.push_back("unpaired_remaining");
    return raw_pairs;
}

} // namespace

PairingResult buildPairingCandidates(const PairingRequest& req)
{
    PairingResult out;
    if(!req.domain || !req.catalog || req.endpoints.size() < 2)
    {
        out.issues.push_back("no_pairs");
        return out;
    }

    std::vector<std::string> ordered = req.endpoints;
    std::vector<std::pair<std::string, std::string>> raw_pairs;

    if(req.topology == ConnectTopology::Chain)
    {
        for(size_t i = 0; i + 1 < ordered.size(); ++i)
        {
            const double sc =
                pairScore(*req.domain, *req.catalog, req.semantics, req.channel_index,
                          ordered[i], ordered[i + 1], req.snapshot_components);
            if(sc < 0.6)
            {
                out.issues.push_back("incompatible_chain_step");
                return out;
            }
            raw_pairs.push_back({ordered[i], ordered[i + 1]});
        }
    }
    else if(req.topology == ConnectTopology::Tree)
    {
        std::string hub;
        if(req.hub_long_name)
            hub = *req.hub_long_name;
        else
        {
            const auto picked = pickAutoHub(req, out);
            if(picked)
                hub = *picked;
            else if(out.issues.empty() && !ordered.empty())
                hub = ordered.front();
            else
                return out;
        }
        for(const std::string& leaf : ordered)
        {
            if(leaf == hub)
                continue;
            raw_pairs.push_back({hub, leaf});
        }
    }
    else
    {
        for(size_t i = 0; i + 1 < ordered.size(); i += 2)
            raw_pairs.push_back({ordered[i], ordered[i + 1]});
        if((ordered.size() % 2) != 0)
            out.issues.push_back("odd_remaining_count");
    }

    if(req.link_count > 0 && static_cast<int>(raw_pairs.size()) > req.link_count)
        raw_pairs.resize(static_cast<size_t>(req.link_count));

    for(const auto& pr : raw_pairs)
    {
        PairingCandidate c;
        c.from_long_name = pr.first;
        c.to_long_name = pr.second;
        c.score = pairScore(*req.domain, *req.catalog, req.semantics, req.channel_index,
                            c.from_long_name, c.to_long_name, req.snapshot_components);
        fillPorts(c, *req.domain, *req.catalog, req.semantics, req.channel_index, req.goal_en,
                  req.prefer_internal_semantics);
        out.pairs.push_back(std::move(c));
    }

    if(out.issues.empty() && out.pairs.empty())
        out.issues.push_back("no_pairs");
    return out;
}

} // namespace RDK::LLM
