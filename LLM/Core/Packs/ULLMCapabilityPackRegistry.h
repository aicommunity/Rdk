#ifndef RDK_LLM_CAPABILITY_PACK_REGISTRY_H
#define RDK_LLM_CAPABILITY_PACK_REGISTRY_H

#include <memory>
#include <vector>

#include "ILLMCapabilityPack.h"

namespace RDK::LLM {

class ULLMCapabilityPackRegistry : public ILLMCapabilityPackRegistry {
public:
    void registerPack(std::unique_ptr<ILLMCapabilityPack> pack) override;
    std::vector<ILLMCapabilityPack*> packs() const override;
    std::vector<std::pair<ILLMCapabilityPack*, PackMatch>>
    rank(const PackTurnSnapshot& snap) const override;

private:
    std::vector<std::unique_ptr<ILLMCapabilityPack>> m_packs;
};

} // namespace RDK::LLM

#endif
