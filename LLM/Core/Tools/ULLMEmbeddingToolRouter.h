#ifndef RDK_ULLM_EMBEDDING_TOOL_ROUTER_H
#define RDK_ULLM_EMBEDDING_TOOL_ROUTER_H

#include "ULLMSearchTools.h"

namespace RDK::LLM {

/// Hybrid tool search: optional tools-embeddings.jsonl index, lexical fallback via ULLMSearchTools.
SearchToolsResult searchToolsHybrid(ULLMToolRegistry& registry, const std::string& query, int top_k);

} // namespace RDK::LLM

#endif
