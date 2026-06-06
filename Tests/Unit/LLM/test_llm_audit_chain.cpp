#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "Observability/ULLMAuditLog.h"

using namespace RDK::LLM;

TEST(LLMAuditChain, VerifyValidChain)
{
    const std::string dir = "/tmp/rdk_llm_audit_test_dir";
    const std::string path = dir + "/audit.jsonl";
    std::filesystem::remove_all(dir);
    {
        ULLMAuditLog log;
        log.setLogDirectory(dir);
        log.append("user_message", {{"n", 1}}, "t1", "s1");
        log.append("tool_invoke_start", {{"tool", "get_net_snapshot"}}, "t1", "s1");
    }
    const AuditVerifyReport report = ULLMAuditLog::verifyJsonlFile(path);
    EXPECT_TRUE(report.ok()) << report.first_error;
    EXPECT_GE(report.lines_verified, 2u);
}
