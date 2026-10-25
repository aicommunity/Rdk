#include "ULLMConnectPlanParsing.h"

#include "ULLMQuantityParser.h"

#include <regex>

namespace RDK::LLM {

namespace {

bool hasRuCue(const std::string& text)
{
    const std::string lower = text;
    return lower.find("связ") != std::string::npos || lower.find("соедин") != std::string::npos
           || lower.find("линк") != std::string::npos || lower.find("подключ") != std::string::npos;
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

bool isValidateConfigurationGoalText(const std::string& text)
{
    const std::string lower = text;
    if(lower.find("validate") != std::string::npos || lower.find("валид") != std::string::npos)
        return true;
    if(lower.find("провер") != std::string::npos
       && (lower.find("конфиг") != std::string::npos || lower.find("project.ini") != std::string::npos))
        return true;
    if((lower.find("configuration") != std::string::npos || lower.find("project.ini") != std::string::npos)
       && (lower.find(" is valid") != std::string::npos || lower.find("check whether") != std::string::npos
           || lower.find("check if") != std::string::npos))
        return true;
    return false;
}

bool isDisconnectGoalText(const std::string& text)
{
    static const std::regex en_re(R"(\b(disconnect|unlink)\b)", std::regex::icase);
    if(std::regex_search(text, en_re))
        return true;
    const std::string lower = text;
    return lower.find("разорви") != std::string::npos || lower.find("разорв") != std::string::npos
           || lower.find("отключ") != std::string::npos;
}

bool isConnectGoalText(const std::string& text)
{
    static const std::regex en_re(R"(\b(connect|link|links)\b)", std::regex::icase);
    if(std::regex_search(text, en_re))
        return true;
    const std::string lower = text;
    if(lower.find("соедин") != std::string::npos || lower.find("подключ") != std::string::npos)
        return true;
    if(isDisconnectGoalText(text))
        return false;
    return hasRuCue(text);
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
        R"(([A-Za-z0-9_./]+)\.([A-Za-z0-9_]+)\s*(->|→|to|на|с)\s*([A-Za-z0-9_./]+)\.([A-Za-z0-9_]+))",
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
            R"(\b([A-Za-z][A-Za-z0-9_./]{1,})\s*(->|→|and|и|to|к)\s*([A-Za-z][A-Za-z0-9_./]{1,})\b)",
            std::regex::icase);
        for(std::sregex_iterator it(goal_en.begin(), goal_en.end(), pair_re), end; it != end;
            ++it)
        {
            const std::string a = (*it)[1].str();
            const std::string delim = (*it)[2].str();
            const std::string b = (*it)[3].str();
            if(a == "connect" || a == "link" || b == "connect" || b == "link")
                continue;
            // English "add X to the model" uses "to" as a preposition, not a connect delimiter.
            if(delim == "to" || delim == "TO")
            {
                static const std::regex add_to_phrase(
                    R"(\badd\b[\s\S]*\bto\s+the\b)", std::regex::icase);
                if(std::regex_search(goal_en, add_to_phrase))
                    continue;
                if(b == "the" || b == "model" || b == "diagram" || b == "schema")
                    continue;
            }
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

    // "подключи PGenerator …" / "connect PGenerator …" → hub when not set via from/hub.
    if(!out.hub_token)
    {
        static const std::regex connect_hub_re(
            R"((?:подключи|подключить|подключил|connect|link)\s+([A-Za-z][A-Za-z0-9_./]{1,}))",
            std::regex::icase);
        if(std::smatch m; std::regex_search(goal_en, m, connect_hub_re))
            out.hub_token = m[1].str();
    }

    // Live analogous reference: "как … к PNeuron" / "также как к PNeuron" / "same way as …".
    // Note: std::regex \w is ASCII-only; use explicit Cyrillic stems, not \w.
    static const std::regex analogous_ref_re(
        R"((?:также\s+как\s+к\s+|как\s+(?:он\s+)?подключен\s+к\s+|как\s+(?:он\s+)?соединен\s+(?:с|к)\s+|как\s+(?:к|to)\s+|same\s+way\s+as\s+(?:(?:it\s+)?(?:is\s+)?connected\s+)?(?:to\s+)?|same\s+as\s+(?:(?:it\s+)?(?:is\s+)?connected\s+)?(?:to\s+)?|like\s+(?:(?:it\s+)?(?:is\s+)?connected\s+)?(?:to\s+))([A-Za-z][A-Za-z0-9_./]{1,}))",
        std::regex::icase);
    if(std::smatch ref_m; std::regex_search(goal_en, ref_m, analogous_ref_re))
    {
        out.analogous_ref_token = ref_m[1].str();
        out.wants_analogous = true;
    }

    static const std::regex all_peers_re(
        R"((ко\s+всем\s+нейрон|всем\s+нейрон|to\s+all\s+neurons|all\s+neurons|ко\s+всем))",
        std::regex::icase);
    if(std::regex_search(goal_en, all_peers_re))
        out.wants_all_class_peers = true;

    static const std::regex session_peers_re(
        R"((к\s+этим\s+нейрон|этим\s+нейрон|these\s+neurons|to\s+these\s+neurons|к\s+этим))",
        std::regex::icase);
    if(std::regex_search(goal_en, session_peers_re))
        out.wants_session_peers = true;

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
