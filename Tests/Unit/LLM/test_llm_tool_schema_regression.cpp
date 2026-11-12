#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Tools/ULLMToolSchemaRegression.h"

TEST(LLMToolSchemaRegression, CoreToolsHaveValidSchemas)
{
    RDK::LLM::ULLMToolRegistry registry;
    RDK::LLM::URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    const auto issues = RDK::LLM::collectToolSchemaIssues(registry);
    for(const auto& issue : issues)
        ADD_FAILURE() << issue.tool_name << ": " << issue.message;
    EXPECT_TRUE(issues.empty());
}
