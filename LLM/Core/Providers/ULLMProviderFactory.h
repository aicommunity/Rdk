#ifndef RDK_ULLM_PROVIDER_FACTORY_H
#define RDK_ULLM_PROVIDER_FACTORY_H

#include <memory>

#include "ILLMProvider.h"

namespace RDK::LLM {

class ULLMProviderFactory {
public:
    static std::unique_ptr<ILLMProvider> create(const LLMProviderProfile& profile);
};

} // namespace RDK::LLM

#endif
