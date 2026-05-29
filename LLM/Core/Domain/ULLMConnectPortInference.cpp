#include "ULLMConnectPortInference.h"

#include "../Context/ULinkPatternCatalog.h"
#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "ULLMConnectPortHeuristics.h"
#include "URdkDomainAccess.h"

#include <algorithm>

namespace RDK::LLM {

namespace {

std::optional<std::string> findPortCaseInsensitive(const std::vector<std::string>& ports,
                                                   const std::string& requested)
{
    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return out;
    };
    const std::string needle = toLower(requested);
    for(const std::string& p : ports)
    {
        if(toLower(p) == needle)
            return p;
    }
    return std::nullopt;
}

nlohmann::json portCandidatesJson(const std::vector<std::string>& ports)
{
    nlohmann::json arr = nlohmann::json::array();
    for(const std::string& port : ports)
        arr.push_back({{"port_name", port}});
    return arr;
}

std::string formatPortListForMessage(const std::vector<std::string>& ports)
{
    if(ports.empty())
        return "(none)";
    std::string out;
    for(size_t i = 0; i < ports.size(); ++i)
    {
        if(i)
            out += ", ";
        out += ports[i];
    }
    return out;
}

} // namespace

ConnectPortInferenceResult inferConnectPorts(nlohmann::json& args, URdkDomainAccess& domain,
                                             ULinkPatternCatalog& catalog, int channel_index,
                                             const ULLMConnectSemanticsCatalog* semantics,
                                             const std::string& goal_en,
                                             bool prefer_internal_semantics)
{
    ConnectPortInferenceResult out;
    const std::string from_ln = args.value("from_long_name", "");
    const std::string to_ln = args.value("to_long_name", "");
    if(from_ln.empty() || to_ln.empty())
    {
        out.ok = true;
        return out;
    }

    std::vector<std::string> from_outputs;
    std::vector<std::string> from_inputs_unused;
    DomainStatus st_from =
        domain.listComponentPubPorts(from_ln, channel_index, from_outputs, from_inputs_unused);
    if(!st_from.ok())
    {
        if(prefer_internal_semantics && semantics && !goal_en.empty())
        {
            std::string hint_from;
            std::string hint_to;
            if(semantics->matchGoalPortHint(goal_en, hint_from, hint_to))
            {
                args["from_property"] = hint_from;
                args["to_property"] = hint_to;
                out.ok = true;
                return out;
            }
        }
        out.error_code = "ENTITY_NOT_FOUND";
        out.message = st_from.message;
        return out;
    }

    std::vector<std::string> to_outputs_unused;
    std::vector<std::string> to_inputs;
    DomainStatus st_to =
        domain.listComponentPubPorts(to_ln, channel_index, to_outputs_unused, to_inputs);
    if(!st_to.ok())
    {
        out.error_code = "ENTITY_NOT_FOUND";
        out.message = st_to.message;
        return out;
    }

    auto trySemanticsFill = [&]() -> bool {
        if(!semantics || semantics->empty())
            return false;
        std::string hint_from;
        std::string hint_to;
        if(!goal_en.empty() && semantics->matchGoalPortHint(goal_en, hint_from, hint_to))
        {
            args["from_property"] = hint_from;
            args["to_property"] = hint_to;
            out.ok = true;
            return true;
        }
        std::string from_class;
        std::string to_class;
        if(!domain.getComponentClassName(from_ln, channel_index, from_class).ok()
           || !domain.getComponentClassName(to_ln, channel_index, to_class).ok())
            return false;
        const auto suggestions = semantics->suggestContainerPair(from_class, to_class, 3);
        if(suggestions.empty())
            return false;
        const double top = suggestions[0].score;
        const double second = suggestions.size() > 1 ? suggestions[1].score : 0.0;
        constexpr double kMinAutoFillScore = 0.6;
        constexpr double kMinScoreGap = 0.2;
        if(top < kMinAutoFillScore || (top - second) < kMinScoreGap)
            return false;
        args["from_property"] = suggestions[0].from_port;
        args["to_property"] = suggestions[0].to_port;
        out.ok = true;
        return true;
    };

    auto tryCatalogFill = [&]() -> bool {
        std::string from_class;
        std::string to_class;
        const DomainStatus fs = domain.getComponentClassName(from_ln, channel_index, from_class);
        const DomainStatus ts = domain.getComponentClassName(to_ln, channel_index, to_class);
        if(!fs.ok() || !ts.ok())
            return false;

        auto candidates = catalog.suggest(from_class, to_class, 3);
        if(candidates.empty() && from_class.find("Neuron") != std::string::npos
           && to_class.find("Neuron") != std::string::npos)
            candidates = catalog.suggest(from_class, to_class, 5);
        if(candidates.empty())
            return false;

        const double top = candidates[0].score;
        const double second = candidates.size() > 1 ? candidates[1].score : 0.0;
        constexpr double kMinAutoFillScore = 0.6;
        constexpr double kMinScoreGap = 0.2;
        if(top < kMinAutoFillScore || (top - second) < kMinScoreGap)
            return false;

        args["from_property"] = candidates[0].from_port;
        args["to_property"] = candidates[0].to_port;
        out.ok = true;
        return true;
    };

    auto resolveField = [&](const std::string& field, const std::string& component_long_name,
                            const std::vector<std::string>& catalog_ports, bool is_output) -> bool {
        if(!args.contains(field) || !args[field].is_string())
            return true;
        std::string value = args[field].get<std::string>();
        const bool generic = isGenericLinkPortName(value);

        if(!value.empty() && !generic)
        {
            if(const std::optional<std::string> exact =
                   findPortCaseInsensitive(catalog_ports, value))
            {
                args[field] = *exact;
                return true;
            }
        }

        if(!value.empty() && !generic)
        {
            out.needs_clarification = true;
            out.error_code = "CONNECT_PORT_NOT_FOUND";
            out.message = "Unknown " + field + " \"" + value + "\" on component "
                          + component_long_name;
            out.clarification = {{"ambiguous", true},
                                 {"kind", "property"},
                                 {"field", field},
                                 {"component_long_name", component_long_name},
                                 {"candidates", portCandidatesJson(catalog_ports)}};
            return false;
        }

        const std::optional<std::string> picked = is_output ? pickPreferredOutputPort(catalog_ports)
                                                             : pickPreferredInputPort(catalog_ports);
        if(picked)
        {
            args[field] = *picked;
            return true;
        }

        out.needs_clarification = true;
        out.error_code = "CONNECT_PORTS_AMBIGUOUS";
        out.message = "Cannot infer " + field + " for " + component_long_name + ". Specify one of: "
                      + formatPortListForMessage(catalog_ports);
        out.clarification = {{"ambiguous", true},
                             {"kind", "property"},
                             {"field", field},
                             {"component_long_name", component_long_name},
                             {"candidates", portCandidatesJson(catalog_ports)}};
        return false;
    };

    if(prefer_internal_semantics && trySemanticsFill())
        return out;

    if(!resolveField("from_property", from_ln, from_outputs, true))
    {
        if(trySemanticsFill() || tryCatalogFill())
            return out;
        return out;
    }
    if(!resolveField("to_property", to_ln, to_inputs, false))
    {
        if(trySemanticsFill() || tryCatalogFill())
            return out;
        return out;
    }

    const std::string fp = args.value("from_property", "");
    const std::string tp = args.value("to_property", "");
    if(isGenericLinkPortName(fp) && isGenericLinkPortName(tp))
    {
        if(trySemanticsFill() || tryCatalogFill())
            return out;
    }

    out.ok = true;
    return out;
}

} // namespace RDK::LLM
