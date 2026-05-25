#ifndef RDK_EMBEDDED_LLAMA_PROVIDER_API_H
#define RDK_EMBEDDED_LLAMA_PROVIDER_API_H

#include <memory>

#include "../Core/LlmTypes.h"
#include "../Core/Providers/ILLMProvider.h"

namespace RDK::LLM {

std::unique_ptr<ILLMProvider> CreateEmbeddedLlamaProvider(const LLMProviderProfile& profile);

} // namespace RDK::LLM

#endif
