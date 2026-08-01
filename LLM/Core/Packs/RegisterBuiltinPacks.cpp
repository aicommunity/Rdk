#include "RegisterBuiltinPacks.h"

#include "Builtin/UPackChannelCalc.h"
#include "ULLMCapabilityPackRegistry.h"

namespace RDK::LLM {

void RegisterBuiltinCapabilityPacks(ILLMCapabilityPackRegistry& registry)
{
    registry.registerPack(std::make_unique<UPackChannelCalc>());
}

} // namespace RDK::LLM
