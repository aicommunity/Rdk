#include "ULLMProviderFactory.h"

#include "UOpenAICompatProvider.h"
#include "ULLMMockProvider.h"
#include "UEmbeddedLlamaProviderStub.h"
#include "UOllamaNativeProvider.h"

#ifdef RDK_LLM_EMBEDDED
#include "../../Embedded/EmbeddedLlamaProviderApi.h"
#endif

namespace RDK::LLM {

std::unique_ptr<ILLMProvider> ULLMProviderFactory::create(const LLMProviderProfile& profile)
{
    switch(profile.kind)
    {
    case LLMProviderKind::Mock:
        return std::make_unique<ULLMMockProvider>();
    case LLMProviderKind::EmbeddedLlama:
#ifdef RDK_LLM_EMBEDDED
        return CreateEmbeddedLlamaProvider(profile);
#else
        return std::make_unique<UEmbeddedLlamaProviderStub>();
#endif
    case LLMProviderKind::OllamaNative:
        return std::make_unique<UOllamaNativeProvider>(profile);
    case LLMProviderKind::OllamaOpenAICompat:
    case LLMProviderKind::OpenAICompat:
    case LLMProviderKind::Custom:
    default:
        return std::make_unique<UOpenAICompatProvider>(profile);
    }
}

} // namespace RDK::LLM
