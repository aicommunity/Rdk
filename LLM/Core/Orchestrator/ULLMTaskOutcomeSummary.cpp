#include "ULLMTaskOutcomeSummary.h"

#include <sstream>

namespace RDK::LLM {

std::string taskOutcomeShortName(const std::string& long_name)
{
    if(long_name.empty())
        return {};
    // Prefer last two path segments for nested endpoints (neuron/synapse).
    const auto last = long_name.find_last_of('/');
    if(last == std::string::npos)
        return long_name;
    const auto prev = long_name.find_last_of('/', last - 1);
    if(prev == std::string::npos)
        return long_name.substr(last + 1);
    // Skip trivial root-like single parent (e.g. Model/PGenerator → PGenerator).
    const std::string parent = long_name.substr(prev + 1, last - prev - 1);
    const std::string leaf = long_name.substr(last + 1);
    if(parent == "Model" || parent == "Root" || parent.empty())
        return leaf;
    return parent + "/" + leaf;
}

std::string formatConnectLinkOutcomeLine(const nlohmann::json& args)
{
    if(!args.is_object())
        return {};
    const std::string from_ln = args.value("from_long_name", "");
    const std::string to_ln = args.value("to_long_name", "");
    const std::string from_p = args.value("from_property", "");
    const std::string to_p = args.value("to_property", "");
    std::string from = taskOutcomeShortName(from_ln);
    std::string to = taskOutcomeShortName(to_ln);
    if(from.empty())
        from = args.value("from_component", "");
    if(to.empty())
        to = args.value("to_component", "");
    if(from.empty() && to.empty() && from_p.empty() && to_p.empty())
        return {};
    std::ostringstream line;
    if(!from.empty() && !from_p.empty())
        line << from << "." << from_p;
    else if(!from.empty())
        line << from;
    else
        line << from_p;
    line << " → ";
    if(!to.empty() && !to_p.empty())
        line << to << "." << to_p;
    else if(!to.empty())
        line << to;
    else
        line << to_p;
    return line.str();
}

std::string formatAddComponentOutcomeLine(const nlohmann::json& args,
                                          const nlohmann::json& result)
{
    const std::string class_name =
        result.is_object() ? result.value("class_name", "")
                           : std::string{};
    const std::string long_name =
        result.is_object() ? result.value("long_name", "") : std::string{};
    const std::string short_name =
        args.is_object() ? args.value("short_name", "") : std::string{};
    const std::string leaf = !short_name.empty() ? short_name : taskOutcomeShortName(long_name);
    const std::string cls =
        !class_name.empty()
            ? class_name
            : (args.is_object() ? args.value("class_name", "") : std::string{});
    if(cls.empty() && leaf.empty() && long_name.empty())
        return {};
    std::ostringstream line;
    if(!cls.empty() && !leaf.empty())
        line << cls << " (" << leaf << ")";
    else if(!cls.empty() && !long_name.empty())
        line << cls << " (" << long_name << ")";
    else if(!leaf.empty())
        line << leaf;
    else if(!long_name.empty())
        line << long_name;
    else
        line << cls;
    return line.str();
}

std::string buildTaskExecuteSummary(const std::vector<std::string>& connect_lines,
                                    const std::vector<std::string>& add_lines,
                                    const std::string& plan_id, std::size_t completed_steps,
                                    bool ok, const std::string& fail_reason)
{
    std::ostringstream summary;
    bool wrote_body = false;
    if(!connect_lines.empty())
    {
        summary << "Connected " << connect_lines.size() << " link(s):";
        for(const std::string& line : connect_lines)
            summary << "\n- " << line;
        wrote_body = true;
    }
    if(!add_lines.empty())
    {
        if(wrote_body)
            summary << "\n";
        summary << "Added " << add_lines.size() << " component(s):";
        for(const std::string& line : add_lines)
            summary << "\n- " << line;
        wrote_body = true;
    }
    if(!wrote_body)
    {
        summary << "Task plan " << plan_id << ": " << completed_steps << " steps completed.";
    }
    else if(!plan_id.empty() || completed_steps > 0)
    {
        summary << "\n(" << plan_id << ": " << completed_steps << " steps)";
    }
    if(!ok && !fail_reason.empty())
    {
        if(wrote_body || completed_steps > 0)
            summary << " ";
        summary << fail_reason;
    }
    return summary.str();
}

} // namespace RDK::LLM
