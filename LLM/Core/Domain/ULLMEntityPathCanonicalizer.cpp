#include "ULLMEntityPathCanonicalizer.h"

#include "ULLMCurrentComponentScope.h"

namespace RDK::LLM {

namespace {

bool looksLikeFullLongName(const std::string& hint)
{
    return hint.find('.') != std::string::npos;
}

std::string parentScopeFromPin(const LLMGuiContextSnapshot& pin)
{
    const std::string diagram = readDiagramScopeLongName(&pin);
    if(!diagram.empty())
        return diagram;
    const CurrentComponentScope cur = readCurrentComponentScope(&pin);
    return cur.valid ? cur.long_name : std::string();
}

} // namespace

std::vector<std::string> entityPathFieldsFromSchema(const nlohmann::json& input_schema)
{
    std::vector<std::string> fields;
    if(!input_schema.contains("properties") || !input_schema["properties"].is_object())
        return fields;
    for(auto it = input_schema["properties"].begin(); it != input_schema["properties"].end(); ++it)
    {
        const std::string semantic = it.value().value("x-llm-semantic", "");
        if(semantic == "entity_long_name")
            fields.push_back(it.key());
    }
    return fields;
}

bool canonicalizeEntityPaths(const std::string& tool_name, nlohmann::json& arguments,
                             URdkDomainAccess& domain, int channel_index,
                             const LLMGuiContextSnapshot& pin,
                             const nlohmann::json& input_schema)
{
    (void)tool_name;
    const std::vector<std::string> fields = entityPathFieldsFromSchema(input_schema);
    if(fields.empty())
        return true;

    const std::string scope = parentScopeFromPin(pin);
    for(const std::string& field : fields)
    {
        if(!arguments.contains(field) || !arguments[field].is_string())
            continue;
        std::string hint = arguments[field].get<std::string>();
        if(hint.empty() || looksLikeFullLongName(hint))
            continue;

        std::string resolved;
        if(domain.resolveComponentLongName(hint, channel_index, resolved, scope).ok()
           || domain.resolveComponentLongName(hint, channel_index, resolved, "").ok())
            arguments[field] = resolved;
    }
    return true;
}

} // namespace RDK::LLM
