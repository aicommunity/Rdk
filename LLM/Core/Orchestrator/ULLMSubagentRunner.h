#ifndef RDK_ULLM_SUBAGENT_RUNNER_H
#define RDK_ULLM_SUBAGENT_RUNNER_H

#include <string>

#include "../Providers/ILLMProvider.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

struct SubagentRunRequest {
    std::string task;
    int max_rounds = 4;
};

struct SubagentRunResult {
    bool ok = true;
    std::string summary;
};

class ULLMSubagentRunner {
public:
    ULLMSubagentRunner(ILLMProvider& provider, ULLMToolRegistry& registry, ULLMToolGateway& gateway);

    SubagentRunResult runExplore(const SubagentRunRequest& req, const std::string& trace_id,
                                 const std::string& session_id);

private:
    ILLMProvider& m_provider;
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
};

} // namespace RDK::LLM

#endif
