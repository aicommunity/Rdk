#include "ULLMContextAcquisitionPolicy.h"

#include "../Domain/URdkDomainAccess.h"
#include "ULLMConnectPlanParsing.h"

#include <cstdlib>
#include <sstream>

namespace RDK::LLM {

namespace {

bool envFlagEnabled(const char* name)
{
    const char* v = std::getenv(name);
    return v && v[0] == '1';
}

MutateSubkind inferMutateSubkind(const std::string& planning_text,
                                 const ConversationState& state)
{
    if(state.pending_tool_arguments)
    {
        const std::string& tool = state.pending_tool_arguments->tool_name;
        if(tool == "connect_components")
            return MutateSubkind::Connect;
        if(tool == "add_component")
            return MutateSubkind::Add;
        if(tool == "set_property")
            return MutateSubkind::Property;
        if(tool == "open_recent" || tool == "list_recent" || tool == "open_project"
           || tool == "save_project")
            return MutateSubkind::Lifecycle;
        return MutateSubkind::Other;
    }
    if(!planning_text.empty())
    {
        if(isConnectGoalText(planning_text))
            return MutateSubkind::Connect;
        const std::string lower = planning_text;
        if(lower.find("add ") != std::string::npos || lower.find("добав") != std::string::npos)
            return MutateSubkind::Add;
    }
    return MutateSubkind::None;
}

std::string classForLongName(URdkDomainAccess& domain, int channel_index,
                             const std::string& long_name)
{
    if(long_name.empty())
        return {};
    std::string class_name;
    if(!domain.getComponentClassName(long_name, channel_index, class_name).ok())
        return {};
    return class_name;
}

void resolveClassPairFromConnectGoal(const ParsedConnectGoal& parsed, URdkDomainAccess& domain,
                                     int channel_index, std::string& from_class,
                                     std::string& to_class, bool& has_pair)
{
    if(parsed.explicit_links.empty())
        return;
    const ConnectLinkSpec& link = parsed.explicit_links.front();
    if(!link.from.token.empty() && from_class.empty())
        from_class = classForLongName(domain, channel_index, link.from.token);
    if(!link.to.token.empty() && to_class.empty())
        to_class = classForLongName(domain, channel_index, link.to.token);
    if(!from_class.empty() && !to_class.empty())
        has_pair = true;
}

void resolveClassPairFromPending(const ConversationState& state, URdkDomainAccess& domain,
                                 int channel_index, std::string& from_class, std::string& to_class,
                                 bool& has_pair)
{
    if(!state.pending_tool_arguments)
        return;
    const nlohmann::json& args = state.pending_tool_arguments->partial_arguments;
    if(!from_class.empty() && !to_class.empty())
        return;
    if(from_class.empty() && args.contains("from_long_name"))
        from_class = classForLongName(domain, channel_index,
                                      args["from_long_name"].get<std::string>());
    if(to_class.empty() && args.contains("to_long_name"))
        to_class =
            classForLongName(domain, channel_index, args["to_long_name"].get<std::string>());
    if(from_class.empty() && args.contains("class_name"))
        from_class = args["class_name"].get<std::string>();
    if(!from_class.empty() && !to_class.empty())
        has_pair = true;
}

std::string buildRetrievalQuery(const std::string& planning_text, const ConversationState& state)
{
    std::ostringstream q;
    if(!planning_text.empty())
        q << planning_text;
    if(!state.last_user_text_en.empty())
    {
        if(!q.str().empty())
            q << ' ';
        q << state.last_user_text_en;
    }
    return q.str();
}

} // namespace

ContextAcquisitionPlan defaultContextAcquisitionPlan(const ConversationState& state,
                                                       const LLMSessionContext& session)
{
    ContextAcquisitionSignals signals;
    signals.intent = LLMIntentKind::Auto;
    return computeContextAcquisitionPlan(state, session, LLMGuiContextSnapshot{}, signals,
                                         LLMContextAcquisitionMode::Auto);
}

ContextAcquisitionPlan computeContextAcquisitionPlan(const ConversationState& state,
                                                     const LLMSessionContext& session,
                                                     const LLMGuiContextSnapshot& gui,
                                                     const ContextAcquisitionSignals& acq_signals,
                                                     LLMContextAcquisitionMode mode)
{
    ContextAcquisitionPlan plan;
    plan.bootstrap_session = !state.session_context_seeded;

    if(mode == LLMContextAcquisitionMode::Minimal)
        return plan;

    const bool has_list_focus =
        !gui.focused_class_name.empty() || !gui.focused_component_long_name.empty();
    const bool has_diagram_scope = !gui.diagram_scope_long_name.empty();
    const bool cold_facts = state.known_facts.empty();

    plan.prefetch_snapshot =
        session.project_loaded && (has_list_focus || has_diagram_scope || cold_facts);

    plan.inject_link_patterns = acq_signals.mutate_subkind == MutateSubkind::Connect
                                && !acq_signals.from_class.empty() && !acq_signals.to_class.empty();
    plan.inject_connect_semantics = plan.inject_link_patterns;

    const bool prefetch_docs_env = envFlagEnabled("NMSDK_LLM_CONTEXT_PREFETCH_DOCS");
    plan.prefetch_docs =
        !acq_signals.retrieval_query.empty()
        && (acq_signals.intent == LLMIntentKind::Query
            || (acq_signals.intent == LLMIntentKind::Mutate && prefetch_docs_env));

    plan.max_known_facts = 12;
    return plan;
}

ContextAcquisitionSignals buildContextAcquisitionSignals(const ConversationState& state,
                                                           const LLMSessionContext& session,
                                                           const LLMGuiContextSnapshot& gui,
                                                           LLMIntentKind intent,
                                                           const std::string& planning_text)
{
    ContextAcquisitionSignals signals;
    signals.intent = intent;
    signals.retrieval_query = buildRetrievalQuery(planning_text, state);
    signals.mutate_subkind =
        intent == LLMIntentKind::Mutate ? inferMutateSubkind(planning_text, state) : MutateSubkind::None;

    if(signals.mutate_subkind != MutateSubkind::Connect)
        return signals;

    URdkDomainAccess domain(nullptr);
    const int channel = session.active_channel_index;

    if(!gui.focused_class_name.empty())
        signals.from_class = gui.focused_class_name;

    const ParsedConnectGoal parsed = parseConnectGoal(planning_text);
    resolveClassPairFromConnectGoal(parsed, domain, channel, signals.from_class, signals.to_class,
                                    signals.has_resolved_component_pair);
    resolveClassPairFromPending(state, domain, channel, signals.from_class, signals.to_class,
                                signals.has_resolved_component_pair);

    if(!signals.from_class.empty() && !signals.to_class.empty())
        signals.has_resolved_component_pair = true;

    return signals;
}

std::string formatKnownFactsBlock(const std::vector<std::string>& facts)
{
    if(facts.empty())
        return {};
    std::ostringstream oss;
    oss << "## Known facts (session)\n";
    for(const std::string& f : facts)
        oss << "- " << f << "\n";
    return oss.str();
}

} // namespace RDK::LLM
