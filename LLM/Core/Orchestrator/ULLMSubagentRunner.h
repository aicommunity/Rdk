#ifndef RDK_ULLM_SUBAGENT_RUNNER_H
#define RDK_ULLM_SUBAGENT_RUNNER_H

#include <string>

#include "../Providers/ILLMProvider.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

struct SubagentRunRequest {
    std::string task;
    /// Profile name (`explore` default). Reserved for future profiles.
    std::string profile = "explore";
    int max_rounds = 4;
};

struct SubagentRunResult {
    bool ok = true;
    std::string summary;
    /// Provider rounds consumed by this run (for session budget accounting).
    int rounds_used = 0;
};

class ULLMSubagentRunner {
public:
    ULLMSubagentRunner(ILLMProvider& provider, ULLMToolRegistry& registry, ULLMToolGateway& gateway);

    SubagentRunResult runExplore(const SubagentRunRequest& req, const std::string& trace_id,
                                 const std::string& session_id,
                                 const LLMSessionContext& session);

private:
    ILLMProvider& m_provider;
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
};

} // namespace RDK::LLM

#endif
