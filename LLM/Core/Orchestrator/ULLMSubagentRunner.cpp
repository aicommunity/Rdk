#include "ULLMSubagentRunner.h"

#include "../LlmModuleInit.h"

namespace RDK::LLM {

ULLMSubagentRunner::ULLMSubagentRunner(ILLMProvider& provider, ULLMToolRegistry& registry,
                                       ULLMToolGateway& gateway)
    : m_provider(provider)
    , m_registry(registry)
    , m_gateway(gateway)
{
}

SubagentRunResult ULLMSubagentRunner::runExplore(const SubagentRunRequest& req,
                                                 const std::string& trace_id,
                                                 const std::string& session_id)
{
    SubagentRunResult out;
    out.summary = "Explore subagent stub: " + req.task.substr(0, 200);
    GetAuditLog().append("subagent_explore_completed",
                         {{"task_len", static_cast<int>(req.task.size())},
                          {"max_rounds", req.max_rounds}},
                         trace_id, session_id);
    (void)m_provider;
    (void)m_registry;
    (void)m_gateway;
    return out;
}

} // namespace RDK::LLM
