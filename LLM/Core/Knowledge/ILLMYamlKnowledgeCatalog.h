#ifndef RDK_ILLM_YAML_KNOWLEDGE_CATALOG_H
#define RDK_ILLM_YAML_KNOWLEDGE_CATALOG_H

#include "../Context/ILLMKnowledgeCatalog.h"

#include <optional>

namespace RDK::LLM {

class ILLMYamlKnowledgeCatalog : public ILLMKnowledgeCatalog {
public:
    virtual ~ILLMYamlKnowledgeCatalog() = default;
};

class UYamlKnowledgeCatalog final : public ILLMYamlKnowledgeCatalog {
public:
    /// Loads YAML-like catalog from disk (no yaml-cpp dependency).
    static std::optional<UYamlKnowledgeCatalog> load(const std::filesystem::path& manifest_path,
                                                     const std::filesystem::path& repository_root,
                                                     std::string* error_out = nullptr);

    std::vector<LLMKnowledgeSource> sources() const override { return m_sources; }
    std::filesystem::path prebuiltIndexDirectory() const override { return m_prebuilt_index_dir; }
    std::string catalogFingerprint() const override;

private:
    std::filesystem::path m_manifest_path;
    std::vector<LLMKnowledgeSource> m_sources;
    std::filesystem::path m_prebuilt_index_dir;
};

} // namespace RDK::LLM

#endif
