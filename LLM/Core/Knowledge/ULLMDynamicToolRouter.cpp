#include "ULLMDynamicToolRouter.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <optional>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

bool containsAny(const std::string& text, const std::initializer_list<const char*> needles)
{
    for(const char* needle : needles)
    {
        if(text.find(needle) != std::string::npos)
            return true;
    }
    return false;
}

std::optional<std::unordered_set<std::string>> preferredToolsForText(const std::string& user_text)
{
    const std::string lower = toLowerAscii(user_text);

    if(containsAny(lower, {"create config", "create configuration", "create project", "new config",
                           "созда", "новый проект", "новую конфигурац"}))
        return std::unordered_set<std::string>{"create_configuration"};

    if(containsAny(lower, {"load config", "open config", "open project", "открой", "загруз"}))
        return std::unordered_set<std::string>{
            "load_configuration", "load_project", "open_recent_configuration"};

    if(containsAny(lower, {"validate", "валид", "провер"}))
        return std::unordered_set<std::string>{"validate_configuration"};

    if(containsAny(lower, {"save config", "save project", "сохран"}))
        return std::unordered_set<std::string>{
            "save_configuration", "save_project", "save_project_metadata"};

    if(containsAny(lower, {"close config", "close project", "закрой"}))
        return std::unordered_set<std::string>{"close_configuration"};

    if(containsAny(lower, {"add component", "add ", "добав", "connect", "link", "соедин",
                           "set property", "свойств"}))
        return std::unordered_set<std::string>{"add_component",          "connect_components",
                                               "set_property",           "set_active_channel",
                                               "describe_class",         "list_registered_classes",
                                               "find_component",         "get_component_properties",
                                               "get_net_snapshot",       "list_channels"};

    return std::nullopt;
}

std::optional<std::unordered_set<std::string>> intersectAllowed(
    const std::optional<std::unordered_set<std::string>>& base,
    const std::unordered_set<std::string>& preferred)
{
    if(!base || preferred.empty())
        return std::nullopt;

    std::unordered_set<std::string> narrowed;
    for(const std::string& name : *base)
    {
        if(preferred.count(name))
            narrowed.insert(name);
    }
    if(narrowed.empty())
        return std::nullopt;
    return narrowed;
}

} // namespace

bool ULLMDynamicToolRouter::isEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");
    return v && v[0] == '1';
}

ToolFilter ULLMDynamicToolRouter::apply(const ToolFilter& base, const std::string& user_text)
{
    if(!isEnabled())
        return base;

    ToolFilter out = base;
    const auto preferred = preferredToolsForText(user_text);
    if(!preferred)
        return out;

    if(const auto narrowed = intersectAllowed(out.allowed_tool_names, *preferred))
        out.allowed_tool_names = *narrowed;
    return out;
}

} // namespace RDK::LLM
