#ifndef RDK_ULLM_CONTEXT_KNOWLEDGE_BLOCKS_H
#define RDK_ULLM_CONTEXT_KNOWLEDGE_BLOCKS_H

#include <string>

namespace RDK::LLM {

class UDocSearchIndex;
class ULinkPatternCatalog;
class ULLMConnectSemanticsCatalog;

std::string buildLinkPatternHintBlock(const ULinkPatternCatalog& cat,
                                    const std::string& from_class, const std::string& to_class,
                                    int top_k);

std::string buildConnectSemanticsHintBlock(const ULLMConnectSemanticsCatalog& sem,
                                           const std::string& from_class,
                                           const std::string& to_class, int top_k);

/// Ephemeral rules for inspect-then-connect (nested subtree anchors).
std::string buildConnectInspectHintBlock();

std::string buildDocsPrefetchBlock(UDocSearchIndex& index, const std::string& query,
                                   const std::string& scope, int top_k, std::size_t max_chars);

} // namespace RDK::LLM

#endif
