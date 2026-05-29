#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMStepVerifier.h"

using namespace RDK::LLM;

TEST(LLMStepVerifier, MultiLinkExistsEmptyIsFalse)
{
    URdkDomainAccess domain(nullptr);
    SuccessCriteria c;
    c.type = "multi_link_exists";
    c.params = {{"links", nlohmann::json::array()}};
    VerifyResult r = verifySuccessCriteria(c, domain, 0);
    EXPECT_FALSE(r.satisfied);
}

