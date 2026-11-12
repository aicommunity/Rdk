#include "LlmModuleInit.h"

#include "Context/ILLMProjectContextProvider.h"
#include "Observability/ULLMAuditLog.h"
#include "Tools/ULLMToolRegistry.h"

namespace RDK::LLM {

ILLMProjectContextProviderRegistry& GetContextRegistry()
{
    static ILLMProjectContextProviderRegistry g_context_registry;
    return g_context_registry;
}

static ULLMToolRegistry g_tool_registry;
static ULLMAuditLog g_audit_log;

ULLMToolRegistry& GetToolRegistry() { return g_tool_registry; }
ULLMAuditLog& GetAuditLog() { return g_audit_log; }

} // namespace RDK::LLM
