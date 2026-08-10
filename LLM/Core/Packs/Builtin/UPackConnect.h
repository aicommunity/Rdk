#ifndef RDK_LLM_PACK_CONNECT_H
#define RDK_LLM_PACK_CONNECT_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-CONN-001/002: connect goal match, hints, and live-analogous recorded execution.
class UPackConnect : public ILLMCapabilityPack {
public:
    const char* id() const override { return "connect"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override;
};

} // namespace RDK::LLM

#endif
