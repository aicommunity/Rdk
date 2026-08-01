#ifndef RDK_LLM_PACK_CONNECT_H
#define RDK_LLM_PACK_CONNECT_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// DD-CONN-001/002: connect goal match + hints. Live-analogous execute stays in orchestrator
/// (TaskPlan-coupled HintOnly FastPath) until a later pack phase.
class UPackConnect : public ILLMCapabilityPack {
public:
    const char* id() const override { return "connect"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
    // tryRecorded: not yet — see TODO in ULLMAgentOrchestrator live_analogous_fastpath.
};

} // namespace RDK::LLM

#endif
