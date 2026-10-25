#ifndef RDK_LLM_PACK_CHANNEL_CALC_H
#define RDK_LLM_PACK_CHANNEL_CALC_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-CALC-001: channel calculation FastPath as a capability pack.
class UPackChannelCalc : public ILLMCapabilityPack {
public:
    const char* id() const override { return "channel_calc"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override;
};

} // namespace RDK::LLM

#endif
