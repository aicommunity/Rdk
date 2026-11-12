#ifndef RDK_LLM_PACK_COMPONENT_STRUCTURE_H
#define RDK_LLM_PACK_COMPONENT_STRUCTURE_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-STRUCT-001: dendrite structure FastPath as a capability pack.
class UPackComponentStructure : public ILLMCapabilityPack {
public:
    const char* id() const override { return "component_structure"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override;
};

} // namespace RDK::LLM

#endif
