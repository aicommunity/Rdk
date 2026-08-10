#ifndef RDK_LLM_REGISTER_BUILTIN_PACKS_H
#define RDK_LLM_REGISTER_BUILTIN_PACKS_H

namespace RDK::LLM {

class ILLMCapabilityPackRegistry;

void RegisterBuiltinCapabilityPacks(ILLMCapabilityPackRegistry& registry);

} // namespace RDK::LLM

#endif
