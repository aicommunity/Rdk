#include "Observability/ULLMAuditLog.h"

#include <iostream>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: llm_audit_verify <audit.jsonl>\n";
        return 2;
    }
    const RDK::LLM::AuditVerifyReport report = RDK::LLM::ULLMAuditLog::verifyJsonlFile(argv[1]);
    std::cout << "verified=" << report.lines_verified << " failed=" << report.lines_failed << '\n';
    if(!report.first_error.empty())
        std::cerr << report.first_error << '\n';
    return report.ok() ? 0 : 1;
}
