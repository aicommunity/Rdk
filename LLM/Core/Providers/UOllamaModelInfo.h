#ifndef RDK_UOLLAMA_MODEL_INFO_H
#define RDK_UOLLAMA_MODEL_INFO_H

#include <string>
#include <vector>

#include "../LlmTypes.h"

namespace RDK::LLM {

OllamaChatTemplateFamily inferTemplateFamilyFromModelfile(const std::string& modelfile);

/// POST /api/show — returns Auto on network/parse failure (caller uses name heuristic).
OllamaChatTemplateFamily fetchOllamaTemplateFamily(const LLMProviderProfile& profile);

/// GET /api/tags — empty on failure.
std::vector<std::string> listOllamaTagModels(const LLMProviderProfile& profile);

/// Human-readable hint when configured model is missing on the server.
std::string formatOllamaModelMismatchHint(const LLMProviderProfile& profile);

/// Env: `NMSDK_LLM_OLLAMA_BASE_URL`, `NMSDK_LLM_OLLAMA_MODEL` (when QSettings override empty).
void applyOllamaEnvironmentDefaults(LLMProviderProfile& profile,
                                    const LLMRuntimeProviderSettings& runtime);

} // namespace RDK::LLM

#endif
