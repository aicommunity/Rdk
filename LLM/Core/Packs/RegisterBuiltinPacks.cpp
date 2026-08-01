#include "RegisterBuiltinPacks.h"

#include "Builtin/UPackAddComponentDirect.h"
#include "Builtin/UPackChannelCalc.h"
#include "Builtin/UPackComponentStructure.h"
#include "Builtin/UPackConnect.h"
#include "Builtin/UPackLifecycleSoft.h"
#include "Builtin/UPackWatchPlot.h"
#include "ULLMCapabilityPackRegistry.h"

namespace RDK::LLM {

void RegisterBuiltinCapabilityPacks(ILLMCapabilityPackRegistry& registry)
{
    // DD-PACK-003 dependency order: mutate packs before channel_calc side-effect.
    registry.registerPack(std::make_unique<UPackAddComponentDirect>());
    registry.registerPack(std::make_unique<UPackComponentStructure>());
    registry.registerPack(std::make_unique<UPackConnect>());
    registry.registerPack(std::make_unique<UPackWatchPlot>());
    registry.registerPack(std::make_unique<UPackChannelCalc>());
    registry.registerPack(std::make_unique<UPackLifecycleSoft>());
}

} // namespace RDK::LLM
