#ifndef RDK_LLM_MODULE_INIT_H
#define RDK_LLM_MODULE_INIT_H

namespace RDK::LLM {

class ULLMToolRegistry;
class ILLMProjectContextProviderRegistry;
class ULLMAuditLog;

ULLMToolRegistry& GetToolRegistry();
ILLMProjectContextProviderRegistry& GetContextRegistry();
ULLMAuditLog& GetAuditLog();

} // namespace RDK::LLM

#endif
