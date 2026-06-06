#include "ULLMWriteToolPolicy.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::string normalizePropertyKey(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool isInLowRiskSet(const std::string& normalized)
{
    static const std::unordered_set<std::string> kLowRisk = {
        "comment",
        "usercomment",
        "description",
        "label",
        "title",
        "tooltip",
        "hint",
        "note",
    };
    return kLowRisk.count(normalized) > 0;
}

} // namespace

bool isLowRiskPropertyName(const std::string& property_name)
{
    if(property_name.empty())
        return false;
    return isInLowRiskSet(normalizePropertyKey(property_name));
}

bool isConfirmationExemptWriteTool(const std::string& tool_name, const nlohmann::json& arguments)
{
    if(tool_name != "set_property")
        return false;
    if(!arguments.contains("property_name") || !arguments.at("property_name").is_string())
        return false;
    return isLowRiskPropertyName(arguments.at("property_name").get<std::string>());
}

} // namespace RDK::LLM
