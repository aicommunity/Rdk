#include "ULLMToolSchemaRegression.h"

namespace RDK::LLM {

std::vector<ToolSchemaIssue> collectToolSchemaIssues(const ULLMToolRegistry& registry)
{
    std::vector<ToolSchemaIssue> issues;
    ToolFilter filter;
    filter.include_write = true;
    filter.intent = LLMIntentKind::Auto;

    for(const LLMToolDefinition& def : registry.listForLlmApi(filter))
    {
        if(def.name.empty())
        {
            issues.push_back({def.name, "empty tool name"});
            continue;
        }
        if(def.description.empty())
            issues.push_back({def.name, "missing description"});
        if(!def.input_schema.is_object())
        {
            issues.push_back({def.name, "input_schema must be object"});
            continue;
        }
        if(def.input_schema.value("type", "") != "object")
            issues.push_back({def.name, "input_schema.type must be object"});
        if(!def.input_schema.contains("properties") && def.input_schema.value("additionalProperties", true))
            issues.push_back({def.name, "input_schema should define properties or forbid additionalProperties"});
    }
    return issues;
}

} // namespace RDK::LLM
