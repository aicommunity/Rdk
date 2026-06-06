#ifndef RDK_ILLM_KNOWLEDGE_CATALOG_H
#define RDK_ILLM_KNOWLEDGE_CATALOG_H

#include <filesystem>
#include <string>
#include <vector>

namespace RDK::LLM {

enum class LLMKnowledgeKind {
    ProductDocs,
    LibraryDocs,
    RdkCoreDocs,
    BinDocs,
    RdkSource,
    LibrarySource,
    AppSource,
};

enum class LLMContentKind { Doc, Source, RuntimeXml };

struct LLMKnowledgeSource {
    std::string source_id;
    std::filesystem::path root;
    LLMKnowledgeKind kind = LLMKnowledgeKind::ProductDocs;
    std::vector<std::string> extensions;
    std::vector<std::string> exclude_globs;
    /// When non-empty, only these file names (relative to root) are indexed.
    std::vector<std::string> include_files_only;
};

class ILLMKnowledgeCatalog {
public:
    virtual ~ILLMKnowledgeCatalog() = default;
    virtual std::vector<LLMKnowledgeSource> sources() const = 0;
    virtual std::filesystem::path prebuiltIndexDirectory() const = 0;
    virtual std::string catalogFingerprint() const = 0;
};

LLMContentKind contentKindForSource(const LLMKnowledgeSource& source);

} // namespace RDK::LLM

#endif
