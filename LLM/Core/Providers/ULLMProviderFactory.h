#ifndef RDK_ULLM_PROVIDER_FACTORY_H
#define RDK_ULLM_PROVIDER_FACTORY_H

#include <memory>

#include "ILLMProvider.h"

namespace RDK::LLM {

using EmbeddedProviderCreatorFn = std::unique_ptr<ILLMProvider> (*)(const LLMProviderProfile&);

class ULLMProviderFactory {
public:
    static std::unique_ptr<ILLMProvider> create(const LLMProviderProfile& profile);
    /// Registered by rdk.llm.embedded at static init (avoids hard link dep in unit tests).
    static void registerEmbeddedCreator(EmbeddedProviderCreatorFn fn);
};

} // namespace RDK::LLM

#endif
