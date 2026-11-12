#ifndef RDK_REGISTER_OBSERVABILITY_TOOLS_H
#define RDK_REGISTER_OBSERVABILITY_TOOLS_H

namespace RDK::LLM {

class ULLMToolRegistry;
class URdkDomainAccess;

void RegisterObservabilityTools(ULLMToolRegistry& registry, URdkDomainAccess& domain);

} // namespace RDK::LLM

#endif
