#ifndef RDK_UOLLAMA_MODEL_INFO_H
#define RDK_UOLLAMA_MODEL_INFO_H

#include "../LlmTypes.h"

namespace RDK::LLM {

OllamaChatTemplateFamily inferTemplateFamilyFromModelfile(const std::string& modelfile);

/// POST /api/show — returns Auto on network/parse failure (caller uses name heuristic).
OllamaChatTemplateFamily fetchOllamaTemplateFamily(const LLMProviderProfile& profile);

} // namespace RDK::LLM

#endif
