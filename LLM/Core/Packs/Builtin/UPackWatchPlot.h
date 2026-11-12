#ifndef RDK_LLM_PACK_WATCH_PLOT_H
#define RDK_LLM_PACK_WATCH_PLOT_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-WATCH-001: watch plot FastPath as a capability pack.
class UPackWatchPlot : public ILLMCapabilityPack {
public:
    const char* id() const override { return "watch_plot"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap) override;
};

} // namespace RDK::LLM

#endif
