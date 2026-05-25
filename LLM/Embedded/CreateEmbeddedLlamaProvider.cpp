#include "EmbeddedLlamaProviderApi.h"

#include "UEmbeddedLlamaProvider.h"

namespace RDK::LLM {

std::unique_ptr<ILLMProvider> CreateEmbeddedLlamaProvider(const LLMProviderProfile& profile)
{
    return std::make_unique<UEmbeddedLlamaProvider>(profile);
}

} // namespace RDK::LLM
