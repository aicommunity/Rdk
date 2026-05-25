#ifndef RDK_ULLM_TOOL_SCHEMA_REGRESSION_H
#define RDK_ULLM_TOOL_SCHEMA_REGRESSION_H

#include <string>
#include <vector>

#include "ULLMToolRegistry.h"

namespace RDK::LLM {

struct ToolSchemaIssue {
    std::string tool_name;
    std::string message;
};

std::vector<ToolSchemaIssue> collectToolSchemaIssues(const ULLMToolRegistry& registry);

} // namespace RDK::LLM

#endif
