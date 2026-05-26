#ifndef RDK_ILLM_YAML_KNOWLEDGE_CATALOG_H
#define RDK_ILLM_YAML_KNOWLEDGE_CATALOG_H

// Post-MVP TD-031: optional yaml-cpp loader implementing ILLMKnowledgeCatalog.
// MVP uses NmsdkBuiltinKnowledgeCatalog (C++). Do not include yaml-cpp here yet.

namespace RDK::LLM {

class ILLMYamlKnowledgeCatalog {
public:
    virtual ~ILLMYamlKnowledgeCatalog() = default;
};

} // namespace RDK::LLM

#endif
