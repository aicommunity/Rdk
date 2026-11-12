#ifndef RDK_REGISTER_PROJECT_KNOWLEDGE_TOOLS_H
#define RDK_REGISTER_PROJECT_KNOWLEDGE_TOOLS_H

namespace RDK::LLM {

class ULLMToolRegistry;

/// Domain-shaped config/docs/artifact tools (Phases E–F): path-policy read-only, no shell/FS write.
void RegisterProjectKnowledgeTools(ULLMToolRegistry& registry);

} // namespace RDK::LLM

#endif
