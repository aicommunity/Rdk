#include "ULLMConnectPlanParsing.h"

#include "ULLMQuantityParser.h"

#include <regex>

namespace RDK::LLM {

namespace {

bool hasRuCue(const std::string& text)
{
    const std::string lower = text;
    return lower.find("связ") != std::string::npos || lower.find("соедин") != std::string::npos
           || lower.find("линк") != std::string::npos;
}

ConnectEndpointRef makeEndpoint(const std::string& token, const std::string& property = "")
{
    ConnectEndpointRef out;
    out.token = token;
    out.property = property;
    out.looks_like_long_name =
        token.find('/') != std::string::npos || token.find('.') != std::string::npos;
    return out;
}

} // namespace

bool isConnectGoalText(const std::string& text)
{
    static const std::regex en_re(R"(\b(connect|link|links)\b)", std::regex::icase);
    return std::regex_search(text, en_re) || hasRuCue(text);
}

ParsedConnectGoal parseConnectGoal(const std::string& goal_en)
{
    ParsedConnectGoal out;
    static const std::regex remaining_re(R"((\bremaining\b|\bleft\b|\brest\b|остал|оставш))",
                                         std::regex::icase);
    static const std::regex analogous_re(
        R"((как|like|same way|по аналогии|аналогичн|предыдущ|previous))", std::regex::icase);
    out.wants_remaining = std::regex_search(goal_en, remaining_re);
    out.wants_analogous = std::regex_search(goal_en, analogous_re);

    static const std::regex explicit_ports(
        R"(([A-Za-z0-9_./]+)\.([A-Za-z0-9_]+)\s*(->|→|to|на)\s*([A-Za-z0-9_./]+)\.([A-Za-z0-9_]+))",
        std::regex::icase);
    for(std::sregex_iterator it(goal_en.begin(), goal_en.end(), explicit_ports), end; it != end;
        ++it)
    {
        ConnectLinkSpec spec;
        spec.from = makeEndpoint((*it)[1].str(), (*it)[2].str());
        spec.to = makeEndpoint((*it)[4].str(), (*it)[5].str());
        out.explicit_links.push_back(std::move(spec));
    }
    if(!out.explicit_links.empty())
        out.kind = ConnectGoalKind::ExplicitPorts;

    if(out.explicit_links.empty())
    {
        static const std::regex pair_re(
            R"(\b([A-Za-z][A-Za-z0-9_./]{1,})\s*(->|→|and|и|to)\s*([A-Za-z][A-Za-z0-9_./]{1,})\b)",
            std::regex::icase);
        for(std::sregex_iterator it(goal_en.begin(), goal_en.end(), pair_re), end; it != end;
            ++it)
        {
            const std::string a = (*it)[1].str();
            const std::string b = (*it)[3].str();
            if(a == "connect" || a == "link" || b == "connect" || b == "link")
                continue;
            ConnectLinkSpec spec;
            spec.from = makeEndpoint(a);
            spec.to = makeEndpoint(b);
            out.explicit_links.push_back(std::move(spec));
        }
        if(!out.explicit_links.empty())
            out.kind = ConnectGoalKind::ExplicitPairs;
    }

    ParsedQuantity q = extractQuantityHeuristic(goal_en);
    if(q.valid)
        out.link_count = q.count;

    static const std::regex model_remaining_re(
        R"((на схеме|в модели|in the model|on the (schema|diagram)|all unlinked|все не связанные))",
        std::regex::icase);
    if(std::regex_search(goal_en, model_remaining_re))
        out.remaining_scope = ConnectRemainingScope::ModelGraph;

    static const std::regex chain_re(R"((цепочк|chain|последовательн))", std::regex::icase);
    static const std::regex tree_re(
        R"((дерев|tree|star|к остальным|to the rest|the rest|hub))", std::regex::icase);
    if(std::regex_search(goal_en, chain_re))
        out.topology = ConnectTopology::Chain;
    else if(std::regex_search(goal_en, tree_re))
        out.topology = ConnectTopology::Tree;

    static const std::regex hub_re(
        R"((?:\bот\b|\bfrom\b|\bhub\b)\s+([A-Za-z][A-Za-z0-9_./]{1,}))", std::regex::icase);
    if(std::smatch hub_m; std::regex_search(goal_en, hub_m, hub_re))
        out.hub_token = hub_m[1].str();

    static const std::regex sem_hint_re(
        R"((низкопорог|ltzone|lt zone|синапс|synapse|threshold|зона))", std::regex::icase);
    if(std::regex_search(goal_en, sem_hint_re))
        out.wants_internal_semantics_hint = true;

    if(out.kind == ConnectGoalKind::None)
    {
        if(out.wants_remaining)
        {
            out.kind = ConnectGoalKind::RemainingSessionDelta;
            if(out.link_count > 1)
                out.kind = ConnectGoalKind::CountOnly;
        }
        else if(out.wants_analogous)
            out.kind = ConnectGoalKind::AnalogousToPrevious;
        else if(out.link_count > 1)
            out.kind = ConnectGoalKind::CountOnly;
        else if(isConnectGoalText(goal_en))
        {
            out.kind = ConnectGoalKind::CountOnly;
            if(out.link_count <= 0)
                out.link_count = 1;
        }
    }
    return out;
}

} // namespace RDK::LLM
