#ifndef RDK_LLM_PACK_ADD_COMPONENT_DIRECT_H
#define RDK_LLM_PACK_ADD_COMPONENT_DIRECT_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-MEM-002/003: direct add_component FastPath as a capability pack.
class UPackAddComponentDirect : public ILLMCapabilityPack {
public:
    const char* id() const override { return "add_component_direct"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override;
};

} // namespace RDK::LLM

#endif
