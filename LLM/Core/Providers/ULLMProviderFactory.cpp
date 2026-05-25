#include "ULLMProviderFactory.h"

#include "UOpenAICompatProvider.h"
#include "ULLMMockProvider.h"
#include "UEmbeddedLlamaProviderStub.h"

namespace RDK::LLM {

std::unique_ptr<ILLMProvider> ULLMProviderFactory::create(const LLMProviderProfile& profile)
{
    switch(profile.kind)
    {
    case LLMProviderKind::Mock:
        return std::make_unique<ULLMMockProvider>();
    case LLMProviderKind::EmbeddedLlama:
        return std::make_unique<UEmbeddedLlamaProviderStub>();
    case LLMProviderKind::OllamaOpenAICompat:
    case LLMProviderKind::OpenAICompat:
    case LLMProviderKind::OllamaNative:
    case LLMProviderKind::Custom:
    default:
        return std::make_unique<UOpenAICompatProvider>(profile);
    }
}

} // namespace RDK::LLM
