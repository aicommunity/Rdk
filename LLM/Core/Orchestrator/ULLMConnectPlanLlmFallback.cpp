#include "ULLMConnectPlanLlmFallback.h"

#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "../Domain/ULLMConnectPortInference.h"
#include "../Domain/ULLMLinkIdentity.h"
#include "../Domain/ULLMModelLinkWalker.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Providers/ILLMProvider.h"

#include <sstream>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::optional<nlohmann::json> parseJsonObjectLenient(const std::string& text)
{
    const size_t start = text.find('{');
    if(start == std::string::npos)
        return std::nullopt;
    const std::string slice = text.substr(start);
    try
    {
        return nlohmann::json::parse(slice);
    }
    catch(...)
    {
        return std::nullopt;
    }
}

} // namespace

ConnectPlanBuildResult tryBuildConnectPlanViaLlm(const ConnectPlanBuildRequest& req,
                                                 ILLMProvider& provider)
{
    ConnectPlanBuildResult out;
    nlohmann::json snap;
    const DomainStatus st = req.domain.listNetSnapshot(snap, req.session.active_channel_index);
    if(!st.ok())
    {
        out.issues.push_back("snapshot_failed");
        return out;
    }

    std::ostringstream user;
    user << "User goal:\n"
         << req.goal_en << "\n\n";
    user << "Components (short_name | long_name | class_name):\n";
    int comp_lines = 0;
    for(const auto& c : snap.value("components", nlohmann::json::array()))
    {
        if(comp_lines++ >= 40)
            break;
        user << c.value("short_name", "") << " | " << c.value("long_name", "") << " | "
             << c.value("class_name", "") << "\n";
    }

    user << "\nReturn JSON only with schema:\n";
    user << "{\"links\":[{\"from_long_name\":\"...\",\"to_long_name\":\"...\",\"from_property\":\"\","
            "\"to_property\":\"\"}]}\n";

    LLMMessage sys;
    sys.role = LLMMessage::Role::System;
    sys.content = "Extract links JSON only. No markdown.";
    LLMMessage umsg;
    umsg.role = LLMMessage::Role::User;
    umsg.content = user.str();

    LLMCompletionOptions opts;
    opts.temperature = 0.0;
    opts.max_tokens = 512;
    LLMCompletionResult res = provider.chat({sys, umsg}, opts);
    if(!res.ok)
    {
        out.issues.push_back("llm_fallback_failed");
        return out;
    }

    const std::optional<nlohmann::json> payload = parseJsonObjectLenient(res.text);
    if(!payload || !payload->is_object())
    {
        out.issues.push_back("llm_fallback_invalid_json");
        return out;
    }
    const nlohmann::json links = payload->value("links", nlohmann::json::array());
    if(!links.is_array())
    {
        out.issues.push_back("llm_fallback_invalid_schema");
        return out;
    }

    nlohmann::json goal_links = nlohmann::json::array();
    std::unordered_set<std::string> seen;
    for(const auto& item : links)
    {
        if(!item.is_object())
            continue;
        LinkQuad quad = linkQuadFromJson(item);
        if(quad.from_long_name.empty() || quad.to_long_name.empty())
            continue;
        if(planSnapshotOrModelHasLink(snap, req.domain, quad, req.session.active_channel_index))
            continue;
        const std::string key = linkQuadDedupKey(quad);
        if(!seen.insert(key).second)
            continue;

        nlohmann::json args = linkQuadToJson(quad);
        args["channel_index"] = req.session.active_channel_index;
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
            out.issues.push_back("connect_ports_ambiguous");
            return out;
        }

        LinkQuad inferred{args.value("from_long_name", ""), args.value("from_property", ""),
                          args.value("to_long_name", ""), args.value("to_property", "")};
        goal_links.push_back(linkQuadToJson(inferred));
        ExecutionPlanStep step;
        step.tool_name = "connect_components";
        step.arguments = args;
        step.depends_on = {req.snapshot_step_id};
        step.repeat_count = 1;
        step.success = SuccessCriteria{"link_exists", linkQuadToJson(inferred)};
        out.steps.push_back(std::move(step));
    }

    if(goal_links.empty())
    {
        out.issues.push_back("all_links_already_exist");
        return out;
    }

    if(goal_links.size() == 1)
        out.goal_success = SuccessCriteria{"link_exists", goal_links[0]};
    else
        out.goal_success = SuccessCriteria{"multi_link_exists", {{"links", goal_links}}};

    out.ok = true;
    return out;
}

} // namespace RDK::LLM
