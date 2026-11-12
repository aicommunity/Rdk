#include <gtest/gtest.h>

#include "Domain/URdkEntityResolver.h"

using namespace RDK::LLM;

TEST(LLMEntityResolver, AmbiguousJsonShape)
{
    URdkDomainAccess domain(nullptr);
    URdkEntityResolver resolver(domain);
    EntityResolutionResult result;
    result.status = EntityResolutionStatus::Ambiguous;
    result.candidates.push_back({"Net/Source1", "MatrixSource", "Source1", 0.8});
    result.candidates.push_back({"Net/Source2", "MatrixSource", "Source2", 0.75});
    nlohmann::json j = resolver.toToolJson(result);
    EXPECT_TRUE(j.value("ambiguous", false));
    EXPECT_EQ(j["candidates"].size(), 2);
}
