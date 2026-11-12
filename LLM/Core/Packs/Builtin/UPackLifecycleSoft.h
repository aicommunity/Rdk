#ifndef RDK_LLM_PACK_LIFECYCLE_SOFT_H
#define RDK_LLM_PACK_LIFECYCLE_SOFT_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// TD-102: soft lifecycle hints only (no recorded load unless env-gated direct is clear).
class UPackLifecycleSoft : public ILLMCapabilityPack {
public:
    const char* id() const override { return "lifecycle_soft"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
};

} // namespace RDK::LLM

#endif
