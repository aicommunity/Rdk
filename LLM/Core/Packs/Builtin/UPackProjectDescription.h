#ifndef RDK_LLM_PACK_PROJECT_DESCRIPTION_H
#define RDK_LLM_PACK_PROJECT_DESCRIPTION_H

#include "../ILLMCapabilityPack.h"

namespace RDK::LLM {

/// Guide: gather model modules via snapshot/inspect before update_configuration description.
class UPackProjectDescription : public ILLMCapabilityPack {
public:
    const char* id() const override { return "project_description"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
};

} // namespace RDK::LLM

#endif
