#include "ULLMProviderFactory.h"

#include "UOpenAICompatProvider.h"
#include "ULLMMockProvider.h"
#include "UEmbeddedLlamaProviderStub.h"
#include "UOllamaNativeProvider.h"

namespace RDK::LLM {

namespace {

EmbeddedProviderCreatorFn g_embedded_creator = nullptr;

} // namespace

void ULLMProviderFactory::registerEmbeddedCreator(EmbeddedProviderCreatorFn fn)
{
    g_embedded_creator = fn;
}

std::unique_ptr<ILLMProvider> ULLMProviderFactory::create(const LLMProviderProfile& profile)
{
    switch(profile.kind)
    {
    case LLMProviderKind::Mock:
        return std::make_unique<ULLMMockProvider>();
    case LLMProviderKind::EmbeddedLlama:
        if(g_embedded_creator)
            return g_embedded_creator(profile);
        return std::make_unique<UEmbeddedLlamaProviderStub>();
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
