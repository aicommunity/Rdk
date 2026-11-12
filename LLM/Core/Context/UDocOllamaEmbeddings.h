#ifndef RDK_UDOC_OLLAMA_EMBEDDINGS_H
#define RDK_UDOC_OLLAMA_EMBEDDINGS_H

#include "ILLMProjectContextProvider.h"

#include <string>
#include <vector>

namespace RDK::LLM {

/// Optional Ollama /api/embeddings re-rank (TD-021). Enable with NMSDK_LLM_DOC_EMBED_OLLAMA=1.
class UDocOllamaEmbeddings {
public:
    static bool enabled();
    static bool embedText(const std::string& text, std::vector<float>& out);
    static void rerank(const std::string& query, std::vector<DocSnippet>& hits, int top_k);
};

} // namespace RDK::LLM

#endif
