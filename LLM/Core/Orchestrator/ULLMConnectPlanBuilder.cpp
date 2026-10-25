#include "ULLMConnectPlanBuilder.h"

#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "../Domain/ULLMConnectPortInference.h"
#include "../Domain/ULLMLinkIdentity.h"
#include "../Domain/ULLMModelLinkWalker.h"
#include "../Domain/ULLMResolvedEntityStore.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Session/ULLMConversationStore.h"
#include "../Domain/ULLMConnectEndpoints.h"
#include "../Orchestrator/ULLMConnectPairing.h"
#include "../Session/ULLMSessionGraphMemory.h"

#include <algorithm>
#include <unordered_set>

namespace RDK::LLM {

namespace {

struct ResolvedPair {
    std::string from_long_name;
    std::string to_long_name;
    std::string from_property;
    std::string to_property;
};

std::optional<std::string> resolveEndpointToken(const std::string& token,
                                                const ConnectPlanBuildRequest& req,
                                                const nlohmann::json& snapshot_components)
{
    if(token.empty())
        return std::nullopt;

    if(token.find('/') != std::string::npos)
    {
        nlohmann::json comp;
        if(req.domain.findComponentByLongName(token, comp, req.session.active_channel_index).ok())
            return token;
    }

    if(req.state)
    {
        if(const std::optional<std::string> resolved =
               lookupResolvedEntity(*req.state, "component", normalizeEntityQuery(token),
                                    req.session.active_channel_index))
            return resolved;
    }

    std::vector<std::string> matches;
    for(const auto& comp : snapshot_components)
    {
        const std::string short_name = comp.value("short_name", "");
        std::string lhs = short_name;
        std::string rhs = token;
        std::transform(lhs.begin(), lhs.end(), lhs.begin(), ::tolower);
        std::transform(rhs.begin(), rhs.end(), rhs.begin(), ::tolower);
        if(lhs == rhs)
            matches.push_back(comp.value("long_name", ""));
    }
    if(matches.size() == 1)
        return matches[0];
    return std::nullopt;
}

std::optional<std::string> classNameFromSnapshot(const nlohmann::json& snapshot_components,
                                                 const std::string& long_name)
{
    for(const auto& comp : snapshot_components)
    {
        if(comp.value("long_name", "") == long_name)
        {
            const std::string cls = comp.value("class_name", "");
            if(!cls.empty())
                return cls;
        }
    }
    return std::nullopt;
}

std::optional<std::string> componentClassName(const ConnectPlanBuildRequest& req,
                                              const nlohmann::json& snapshot_components,
                                              const std::string& long_name)
{
    std::string cls;
    if(req.domain.getComponentClassName(long_name, req.session.active_channel_index, cls).ok()
       && !cls.empty())
        return cls;
    return classNameFromSnapshot(snapshot_components, long_name);
}

bool applyAnalogousTemplatePorts(nlohmann::json& args, const ConnectTemplate& tmpl,
                                 const ConnectPlanBuildRequest& req,
                                 const nlohmann::json& snapshot_components)
{
    const std::string from_ln = args.value("from_long_name", "");
    const std::string to_ln = args.value("to_long_name", "");
    const auto from_class = componentClassName(req, snapshot_components, from_ln);
    const auto to_class = componentClassName(req, snapshot_components, to_ln);
    if(!from_class || !to_class)
        return false;
    if(*from_class != tmpl.from_class || *to_class != tmpl.to_class)
        return false;
    if(tmpl.from_port.empty() || tmpl.to_port.empty())
        return false;
    args["from_property"] = tmpl.from_port;
    args["to_property"] = tmpl.to_port;
    return true;
}

bool needsSessionDelta(const ParsedConnectGoal& parsed)
{
    return parsed.kind == ConnectGoalKind::RemainingSessionDelta
           || parsed.kind == ConnectGoalKind::AnalogousToPrevious
           || parsed.kind == ConnectGoalKind::CountOnly
           || parsed.remaining_scope == ConnectRemainingScope::ModelGraph;
}

bool snapshotHasLongName(const nlohmann::json& snapshot_components, const std::string& long_name)
{
    for(const auto& comp : snapshot_components)
    {
        if(comp.value("long_name", "") == long_name)
            return true;
    }
    return false;
}

std::vector<LinkQuad> collectSnapshotLinks(const nlohmann::json& snap)
{
    std::vector<LinkQuad> out;
    if(!snap.contains("links") || !snap["links"].is_array())
        return out;
    for(const auto& link : snap["links"])
        out.push_back(linkQuadFromJson(link));
    return out;
}

std::vector<LinkQuad> collectTemplateLinksBetweenAnchors(const std::vector<LinkQuad>& links,
                                                         const std::string& hub_ln,
                                                         const std::string& ref_ln)
{
    std::vector<LinkQuad> out;
    for(const LinkQuad& q : links)
    {
        const bool from_hub = endpointInSubtree(q.from_long_name, hub_ln);
        const bool to_hub = endpointInSubtree(q.to_long_name, hub_ln);
        const bool from_ref = endpointInSubtree(q.from_long_name, ref_ln);
        const bool to_ref = endpointInSubtree(q.to_long_name, ref_ln);
        if((from_hub && to_ref) || (from_ref && to_hub))
            out.push_back(q);
    }
    return out;
}

std::vector<std::string> collectClassPeerTargets(const nlohmann::json& snapshot_components,
                                                 const std::string& ref_ln,
                                                 const std::string& ref_class)
{
    std::vector<std::string> targets;
    if(ref_class.empty())
        return targets;
    for(const auto& comp : snapshot_components)
    {
        const std::string ln = comp.value("long_name", "");
        if(ln.empty() || ln == ref_ln)
            continue;
        if(endpointInSubtree(ln, ref_ln))
            continue;
        if(comp.value("class_name", "") != ref_class)
            continue;
        targets.push_back(ln);
    }

    std::vector<std::string> roots;
    for(const std::string& ln : targets)
    {
        bool nested_under_peer = false;
        for(const std::string& other : targets)
        {
            if(other == ln)
                continue;
            if(endpointInSubtree(ln, other))
            {
                nested_under_peer = true;
                break;
            }
        }
        if(!nested_under_peer)
            roots.push_back(ln);
    }
    std::sort(roots.begin(), roots.end());
    return roots;
}

std::vector<std::string> collectSessionPeerTargets(const ConversationState* state,
                                                   const nlohmann::json& snapshot_components,
                                                   const std::string& ref_ln,
                                                   const std::string& ref_class)
{
    std::vector<std::string> targets;
    if(!state || ref_class.empty())
        return targets;
    for(const std::string& ln : state->session_graph.added_long_names)
    {
        if(ln.empty() || ln == ref_ln)
            continue;
        if(endpointInSubtree(ln, ref_ln))
            continue;
        const auto cls = classNameFromSnapshot(snapshot_components, ln);
        if(!cls || *cls != ref_class)
            continue;
        // Skip nested under another added peer.
        bool nested = false;
        for(const std::string& other : state->session_graph.added_long_names)
        {
            if(other != ln && endpointInSubtree(ln, other))
            {
                nested = true;
                break;
            }
        }
        if(!nested)
            targets.push_back(ln);
    }
    std::sort(targets.begin(), targets.end());
    return targets;
}

std::string mapTemplateEndpointToTarget(const std::string& endpoint, const std::string& hub_ln,
                                        const std::string& ref_ln, const std::string& target_ln)
{
    (void)hub_ln;
    if(endpointInSubtree(endpoint, ref_ln))
        return mapSubtreeEndpoint(endpoint, ref_ln, target_ln);
    return endpoint;
}

bool buildLiveAnalogousPairs(const ConnectPlanBuildRequest& req, const nlohmann::json& snap,
                             const nlohmann::json& snapshot_components,
                             std::vector<ResolvedPair>& pairs, std::vector<std::string>& issues)
{
    if(!req.parsed.analogous_ref_token)
        return false;

    if(!req.parsed.hub_token)
    {
        issues.push_back("hub_token_required_for_live_analogous");
        return true;
    }

    const auto hub_ln = resolveEndpointToken(*req.parsed.hub_token, req, snapshot_components);
    const auto ref_ln =
        resolveEndpointToken(*req.parsed.analogous_ref_token, req, snapshot_components);
    if(!hub_ln)
    {
        issues.push_back("component_not_found:" + *req.parsed.hub_token);
        return true;
    }
    if(!ref_ln)
    {
        issues.push_back("component_not_found:" + *req.parsed.analogous_ref_token);
        return true;
    }

    std::vector<LinkQuad> all_links = collectSnapshotLinks(snap);
    std::vector<LinkQuad> template_links =
        collectTemplateLinksBetweenAnchors(all_links, *hub_ln, *ref_ln);

    if(template_links.empty() && snap.value("links_truncated", false))
    {
        nlohmann::json listed;
        ModelLinkListFilters filters;
        filters.from_long_name = *hub_ln;
        filters.to_long_name = *ref_ln;
        if(req.domain.listModelLinks(listed, req.session.active_channel_index, "", 0, 2000, filters)
               .ok()
           && listed.contains("links") && listed["links"].is_array())
        {
            for(const auto& link : listed["links"])
                template_links.push_back(linkQuadFromJson(link));
        }
        filters.from_long_name = *ref_ln;
        filters.to_long_name = *hub_ln;
        if(req.domain.listModelLinks(listed, req.session.active_channel_index, "", 0, 2000, filters)
               .ok()
           && listed.contains("links") && listed["links"].is_array())
        {
            for(const auto& link : listed["links"])
                template_links.push_back(linkQuadFromJson(link));
        }
        template_links = dedupeModelLinkQuads(template_links);
        template_links = collectTemplateLinksBetweenAnchors(template_links, *hub_ln, *ref_ln);
    }

    if(template_links.empty())
    {
        // Fallback: incident on ref with other end under hub via component filter.
        ModelLinkListFilters incident;
        incident.component_long_name = *ref_ln;
        nlohmann::json listed;
        if(req.domain.listModelLinks(listed, req.session.active_channel_index, "", 0, 2000, incident)
               .ok()
           && listed.contains("links") && listed["links"].is_array())
        {
            for(const auto& link : listed["links"])
                all_links.push_back(linkQuadFromJson(link));
            template_links = collectTemplateLinksBetweenAnchors(all_links, *hub_ln, *ref_ln);
        }
    }

    if(template_links.empty())
    {
        issues.push_back("no_template_links");
        return true;
    }

    const auto ref_class = componentClassName(req, snapshot_components, *ref_ln);
    if(!ref_class || ref_class->empty())
    {
        issues.push_back("ref_class_unknown");
        return true;
    }

    std::vector<std::string> targets;
    if(req.parsed.wants_session_peers)
        targets = collectSessionPeerTargets(req.state, snapshot_components, *ref_ln, *ref_class);
    if(targets.empty())
        targets = collectClassPeerTargets(snapshot_components, *ref_ln, *ref_class);
    if(targets.empty())
    {
        issues.push_back("no_peer_targets");
        return true;
    }
    (void)req.parsed.wants_all_class_peers;

    for(const LinkQuad& tmpl : template_links)
    {
        for(const std::string& target_ln : targets)
        {
            ResolvedPair pair;
            pair.from_long_name =
                mapTemplateEndpointToTarget(tmpl.from_long_name, *hub_ln, *ref_ln, target_ln);
            pair.to_long_name =
                mapTemplateEndpointToTarget(tmpl.to_long_name, *hub_ln, *ref_ln, target_ln);
            pair.from_property = tmpl.from_property;
            pair.to_property = tmpl.to_property;

            auto endpoint_ok = [&](const std::string& mapped, const std::string& original) {
                if(snapshotHasLongName(snapshot_components, mapped))
                    return true;
                // Hub (or nested under hub) often omitted from shallow lists — allow.
                if(endpointInSubtree(original, *hub_ln) || endpointInSubtree(mapped, *hub_ln))
                    return true;
                // Target root itself may be enough when mapping to the root.
                if(mapped == target_ln)
                    return true;
                // Nested under target: require presence when any nested comps for target exist.
                if(endpointInSubtree(mapped, target_ln))
                {
                    bool any_nested = false;
                    for(const auto& comp : snapshot_components)
                    {
                        const std::string ln = comp.value("long_name", "");
                        if(ln != target_ln && endpointInSubtree(ln, target_ln))
                        {
                            any_nested = true;
                            break;
                        }
                    }
                    if(!any_nested)
                        return true; // isomorphic nested assumed
                    return false;
                }
                return false;
            };

            if(!endpoint_ok(pair.from_long_name, tmpl.from_long_name))
            {
                issues.push_back("target_endpoint_missing:" + pair.from_long_name);
                continue;
            }
            if(!endpoint_ok(pair.to_long_name, tmpl.to_long_name))
            {
                issues.push_back("target_endpoint_missing:" + pair.to_long_name);
                continue;
            }

            pairs.push_back(std::move(pair));
        }
    }

    if(pairs.empty() && issues.empty())
        issues.push_back("no_pairs");
    return true;
}

} // namespace

ConnectPlanBuildResult buildConnectPlanSteps(const ConnectPlanBuildRequest& req)
{
    ConnectPlanBuildResult out;
    nlohmann::json snap;
    if(req.snapshot_override)
        snap = *req.snapshot_override;
    else
    {
        const DomainStatus st = req.domain.listNetSnapshot(snap, req.session.active_channel_index);
        if(!st.ok())
        {
            out.issues.push_back("snapshot_failed");
            return out;
        }
    }

    const nlohmann::json& snapshot_components =
        snap.contains("components") && snap["components"].is_array() ? snap["components"]
                                                                    : nlohmann::json::array();

    std::vector<ResolvedPair> pairs;

    if(req.parsed.analogous_ref_token)
    {
        if(buildLiveAnalogousPairs(req, snap, snapshot_components, pairs, out.issues))
        {
            if(!out.issues.empty() && pairs.empty())
                return out;
        }
    }
    else if(req.parsed.wants_analogous && req.state && !req.state->session_graph.last_template)
    {
        out.issues.push_back("no_previous_template");
        return out;
    }

    if(pairs.empty() && !req.parsed.analogous_ref_token)
    {
        if(!req.parsed.explicit_links.empty())
        {
            for(const ConnectLinkSpec& spec : req.parsed.explicit_links)
            {
                const auto from_ln = resolveEndpointToken(spec.from.token, req, snapshot_components);
                const auto to_ln = resolveEndpointToken(spec.to.token, req, snapshot_components);
                if(!from_ln)
                {
                    out.issues.push_back("component_not_found:" + spec.from.token);
                    continue;
                }
                if(!to_ln)
                {
                    out.issues.push_back("component_not_found:" + spec.to.token);
                    continue;
                }
                pairs.push_back({*from_ln, *to_ln, spec.from.property, spec.to.property});
            }
        }
        else if(needsSessionDelta(req.parsed))
        {
            if(req.parsed.remaining_scope == ConnectRemainingScope::SessionDelta && !req.state)
            {
                out.issues.push_back("no_session_state");
                return out;
            }
            bool links_incomplete = false;
            std::vector<std::string> remaining =
                collectRemainingEndpoints(req.domain, snapshot_components, req.parsed, req.state,
                                          req.session.active_channel_index, links_incomplete);
            if(links_incomplete)
            {
                out.issues.push_back("links_incomplete_for_global_remaining");
                return out;
            }
            if(req.parsed.link_count > 0 && req.parsed.topology == ConnectTopology::Sequential)
            {
                const size_t need = static_cast<size_t>(req.parsed.link_count * 2);
                if(remaining.size() < need)
                {
                    out.issues.push_back("insufficient_remaining");
                    return out;
                }
                remaining.resize(need);
            }
            if(remaining.size() < 2 && req.parsed.topology != ConnectTopology::Tree)
            {
                out.issues.push_back("no_remaining");
                return out;
            }

            PairingRequest pr;
            pr.endpoints = remaining;
            pr.snapshot_components = &snapshot_components;
            pr.domain = &req.domain;
            pr.catalog = &req.catalog;
            pr.semantics = &defaultConnectSemanticsCatalog();
            pr.channel_index = req.session.active_channel_index;
            pr.topology = req.parsed.topology;
            pr.link_count = req.parsed.link_count;
            pr.goal_en = req.goal_en;
            pr.prefer_internal_semantics = req.parsed.wants_internal_semantics_hint;
            if(req.parsed.hub_token)
            {
                const auto hub_ln =
                    resolveEndpointToken(*req.parsed.hub_token, req, snapshot_components);
                if(hub_ln)
                    pr.hub_long_name = *hub_ln;
                else
                {
                    out.issues.push_back("component_not_found:" + *req.parsed.hub_token);
                    return out;
                }
            }

            const PairingResult pr_result = buildPairingCandidates(pr);
            if(!pr_result.issues.empty())
            {
                out.issues = pr_result.issues;
                return out;
            }
            for(const PairingCandidate& c : pr_result.pairs)
                pairs.push_back({c.from_long_name, c.to_long_name, c.from_property, c.to_property});
        }
    }

    // Soft issues from live map (missing endpoints) should not block other successful pairs.
    if(pairs.empty())
    {
        if(out.issues.empty())
            out.issues.push_back("no_pairs");
        return out;
    }
    // Clear soft missing-endpoint notes when we still have pairs; keep hard errors only if empty.
    if(!pairs.empty())
    {
        std::vector<std::string> hard;
        for(const std::string& issue : out.issues)
        {
            if(issue.rfind("target_endpoint_missing:", 0) != 0)
                hard.push_back(issue);
        }
        // If only soft issues, proceed.
        if(hard.empty())
            out.issues.clear();
        else
            out.issues = std::move(hard);
    }
    if(!out.issues.empty() && pairs.empty())
        return out;

    const bool check_new_link_quota =
        req.parsed.kind == ConnectGoalKind::CountOnly && req.parsed.link_count > 0
        && !req.parsed.analogous_ref_token;
    int new_links_planned = 0;

    std::unordered_set<std::string> seen;
    std::vector<LinkQuad> goal_links;
    for(const ResolvedPair& p : pairs)
    {
        nlohmann::json args = {{"from_long_name", p.from_long_name},
                               {"to_long_name", p.to_long_name},
                               {"from_property", p.from_property},
                               {"to_property", p.to_property},
                               {"channel_index", req.session.active_channel_index}};

        const bool has_explicit_ports = !p.from_property.empty() && !p.to_property.empty();
        if(!has_explicit_ports && req.parsed.wants_analogous && !req.parsed.analogous_ref_token
           && req.state && req.state->session_graph.last_template)
        {
            applyAnalogousTemplatePorts(args, *req.state->session_graph.last_template, req,
                                        snapshot_components);
        }

        const bool ports_ready = !args.value("from_property", "").empty()
                                 && !args.value("to_property", "").empty();
        if(!ports_ready)
        {
            const ULLMConnectSemanticsCatalog& semantics = defaultConnectSemanticsCatalog();
            ConnectPortInferenceResult inf = inferConnectPorts(
                args, req.domain, req.catalog, req.session.active_channel_index,
                semantics.empty() ? nullptr : &semantics, req.goal_en,
                req.parsed.wants_internal_semantics_hint);
            if(!inf.ok)
            {
                out.needs_clarification = inf.needs_clarification;
                out.clarification_message = inf.message;
                out.clarification_payload = inf.clarification;
                out.issues.push_back(inf.error_code.empty() ? "connect_ports_ambiguous"
                                                             : inf.error_code);
                return out;
            }
        }

        LinkQuad quad{args.value("from_long_name", ""), args.value("from_property", ""),
                      args.value("to_long_name", ""), args.value("to_property", "")};
        goal_links.push_back(quad);

        const std::string key = linkQuadDedupKey(quad);
        if(!seen.insert(key).second)
            continue;

        const bool already_in_snapshot =
            planSnapshotOrModelHasLink(snap, req.domain, quad, req.session.active_channel_index);
        if(check_new_link_quota && !already_in_snapshot)
            ++new_links_planned;
        if(already_in_snapshot)
            continue;

        ExecutionPlanStep step;
        step.tool_name = "connect_components";
        step.arguments = args;
        step.depends_on = {req.snapshot_step_id};
        step.repeat_count = 1;
        step.success = SuccessCriteria{"link_exists", linkQuadToJson(quad)};
        out.steps.push_back(std::move(step));
    }

    if(check_new_link_quota && new_links_planned < req.parsed.link_count)
    {
        out.issues.push_back("insufficient_new_links");
        return out;
    }

    nlohmann::json links = nlohmann::json::array();
    for(const LinkQuad& q : goal_links)
        links.push_back(linkQuadToJson(q));
    if(links.size() == 1)
        out.goal_success = SuccessCriteria{"link_exists", links[0]};
    else if(!links.empty())
        out.goal_success = SuccessCriteria{"multi_link_exists", {{"links", links}}};

    out.ok = true;
    return out;
}

} // namespace RDK::LLM
