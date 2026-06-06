#include <gtest/gtest.h>

#include "Observability/ULLMAuditSanitizer.h"

TEST(LLMAuditSanitizer, RedactsPromptFields)
{
    nlohmann::json details = {{"user_text", "secret prompt"}, {"nested", {{"content", "body"}}}};
    const nlohmann::json sanitized = RDK::LLM::sanitizeAuditDetails(details);
    EXPECT_NE(sanitized["user_text"].get<std::string>().find("redacted"), std::string::npos);
    EXPECT_NE(sanitized["nested"]["content"].get<std::string>().find("redacted"), std::string::npos);
}
