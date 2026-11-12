#ifndef RDK_REGISTER_CORE_RDK_TOOLS_H
#define RDK_REGISTER_CORE_RDK_TOOLS_H

namespace RDK::LLM {

class ULLMToolRegistry;
class URdkDomainAccess;
class ILLMProjectContextProvider;

void RegisterCoreRdkTools(ULLMToolRegistry& registry, URdkDomainAccess& domain,
                          ILLMProjectContextProvider* project_context);

} // namespace RDK::LLM

#endif
