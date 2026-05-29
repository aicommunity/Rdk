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

    if(req.parsed.wants_analogous && req.state && !req.state->session_graph.last_template)
    {
        out.issues.push_back("no_previous_template");
        return out;
    }

    std::vector<ResolvedPair> pairs;
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

    if(!out.issues.empty())
        return out;
    if(pairs.empty())
    {
        out.issues.push_back("no_pairs");
        return out;
    }

    const bool check_new_link_quota =
        req.parsed.kind == ConnectGoalKind::CountOnly && req.parsed.link_count > 0;
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
        if(!has_explicit_ports && req.parsed.wants_analogous && req.state
           && req.state->session_graph.last_template)
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
