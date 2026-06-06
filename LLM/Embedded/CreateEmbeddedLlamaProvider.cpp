#include "EmbeddedLlamaProviderApi.h"

#include "../Core/Providers/ULLMProviderFactory.h"
#include "UEmbeddedLlamaProvider.h"

namespace RDK::LLM {

std::unique_ptr<ILLMProvider> CreateEmbeddedLlamaProvider(const LLMProviderProfile& profile)
{
    return std::make_unique<UEmbeddedLlamaProvider>(profile);
}

void EnsureEmbeddedLlamaProviderRegistered()
{
    static bool registered = false;
    if(registered)
        return;
    ULLMProviderFactory::registerEmbeddedCreator(&CreateEmbeddedLlamaProvider);
    registered = true;
}

namespace {

struct EmbeddedProviderRegistrar {
    EmbeddedProviderRegistrar() { EnsureEmbeddedLlamaProviderRegistered(); }
};

const EmbeddedProviderRegistrar g_embedded_provider_registrar;

} // namespace

} // namespace RDK::LLM
