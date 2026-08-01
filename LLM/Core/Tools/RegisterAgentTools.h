#ifndef RDK_REGISTER_AGENT_TOOLS_H
#define RDK_REGISTER_AGENT_TOOLS_H

namespace RDK::LLM {

class ULLMToolRegistry;
class ILLMProvider;
class ULLMToolGateway;

void RegisterAgentTools(ULLMToolRegistry& registry);

/// Bind real `spawn_explore_subagent` runner after provider + gateway exist (TD-164).
void bindSpawnExploreSubagent(ILLMProvider& provider, ULLMToolRegistry& registry,
                              ULLMToolGateway& gateway);

} // namespace RDK::LLM

#endif
