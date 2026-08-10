#include "ULLMClarificationFormat.h"

#include <algorithm>
#include <sstream>

namespace RDK::LLM {

void enrichClassDisambiguationWithLastAdd(nlohmann::json& disambiguation,
                                          const std::string& last_added_class)
{
    if(last_added_class.empty())
        return;
    if(!disambiguation.is_object())
        disambiguation = nlohmann::json::object();
    if(disambiguation.value("kind", "") != "class" && disambiguation.value("kind", "") != "")
        return;
    disambiguation["kind"] = "class";
    disambiguation["last_added_class"] = last_added_class;

    nlohmann::json candidates = nlohmann::json::array();
    if(disambiguation.contains("candidates") && disambiguation["candidates"].is_array())
        candidates = disambiguation["candidates"];

    for(const auto& c : candidates)
    {
        if(!c.is_object())
            continue;
        if(c.value("class_name", "") == last_added_class)
        {
            disambiguation["candidates"] = std::move(candidates);
            return;
        }
    }

    nlohmann::json first = nlohmann::json::object();
    first["class_name"] = last_added_class;
    first["label"] = "same as last";
    first["score"] = 1.0;
    nlohmann::json prepended = nlohmann::json::array();
    prepended.push_back(std::move(first));
    for(auto& c : candidates)
        prepended.push_back(std::move(c));
    disambiguation["candidates"] = std::move(prepended);
}

std::string formatClarificationMessage(const nlohmann::json& payload, bool include_candidate_list)
{
    if(!payload.is_object())
        return "I need a clarification to continue. Please provide more detail.";

    std::ostringstream oss;
    const std::string kind = payload.value("kind", "component");
    const std::string field = payload.value("field", "");
    const nlohmann::json candidates = payload.contains("candidates") && payload["candidates"].is_array()
                                          ? payload["candidates"]
                                          : nlohmann::json::array();
    const std::string last_added = payload.value("last_added_class", "");

    if(kind == "property")
    {
        const std::string prop_field = payload.value("field", "property");
        const std::string component = payload.value("component_long_name", "");
        oss << "I need the exact link port for **" << prop_field << "**";
        if(!component.empty())
            oss << " on `" << component << "`";
        oss << ". Choose one:";
        if(include_candidate_list)
        {
            oss << "\n";
            int index = 1;
            for(const auto& c : candidates)
            {
                if(!c.is_object())
                    continue;
                oss << index++ << ". " << c.value("port_name", c.value("name", "")) << "\n";
            }
            oss << "\nReply with the exact port name (e.g. `PortName`).";
        }
        return oss.str();
    }

    if(kind == "class")
    {
        if(!last_added.empty())
        {
            oss << "Same as last time: `" << last_added
                << "` (reply 1), or choose another class.";
        }
        else
        {
            oss << "I couldn't determine the exact component class. Please choose one and reply with "
                   "the exact class name.";
        }
        if(include_candidate_list)
        {
            oss << "\n";
            int index = 1;
            for(const auto& c : candidates)
            {
                if(!c.is_object())
                    continue;
                const std::string cn = c.value("class_name", "");
                oss << index++ << ". " << cn;
                if(c.value("label", "") == "same as last" || (!last_added.empty() && cn == last_added))
                    oss << " (same as last)";
                oss << "\n";
            }
            const size_t n = static_cast<size_t>(std::max(0, index - 1));
            oss << "\nReply with a number (1";
            if(n > 1)
                oss << "-" << n;
            oss << ") or the exact class name (e.g. `NLPNeuron`).";
        }
        return oss.str();
    }

    if(field == "parent_long_name")
    {
        oss << "Where should I add the new component? Choose a parent container.";
        if(include_candidate_list)
        {
            oss << "\n";
            int index = 1;
            for(const auto& c : candidates)
            {
                if(!c.is_object())
                    continue;
                oss << index++ << ". " << c.value("long_name", "") << " ("
                    << c.value("class_name", "") << " / " << c.value("short_name", "") << ")\n";
            }
        }
        return oss.str();
    }

    oss << "Multiple components match. Please specify which one.";
    if(include_candidate_list)
    {
        oss << "\n";
        int index = 1;
        for(const auto& c : candidates)
        {
            if(!c.is_object())
                continue;
            oss << index++ << ". " << c.value("long_name", "") << " ("
                << c.value("class_name", "") << " / " << c.value("short_name", "") << ")\n";
        }
    }
    return oss.str();
}

} // namespace RDK::LLM
