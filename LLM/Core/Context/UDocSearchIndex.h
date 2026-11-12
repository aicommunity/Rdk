#ifndef RDK_UDOC_SEARCH_INDEX_H
#define RDK_UDOC_SEARCH_INDEX_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "ILLMKnowledgeCatalog.h"
#include "ILLMProjectContextProvider.h"
#include "UDocSearchCatalog.h"

namespace RDK::LLM {

constexpr int kIndexMaxFiles = 2500;
constexpr int kSourceExcerptMaxLines = 120;
constexpr double kMinRetrievalScore = 0.12;
constexpr double kIncrementalRebuildRatio = 0.25;

enum class IndexSyncResult {
    UpToDate,
    IncrementalUpdated,
    NeedsFullRebuild,
};

/// TF-IDF + offline random-projection semantic boost (TD-017 hybrid search).
class UDocSearchIndex {
public:
    void build(const std::vector<std::filesystem::path>& roots, int max_files = 800);
    void buildFromCatalog(const ILLMKnowledgeCatalog& catalog,
                          const std::filesystem::path& repository_root,
                          int max_files = kIndexMaxFiles);
    /// Empty `expected_fingerprint` skips fingerprint match (accept stale index for fast startup).
    bool loadPrebuilt(const std::filesystem::path& dir, const std::string& expected_fingerprint);
    void savePrebuilt(const std::filesystem::path& dir, const std::string& fingerprint) const;
    std::vector<DocSnippet> search(const std::string& query, int top_k,
                                   LLMContentKind kind_filter = LLMContentKind::Doc) const;
    std::vector<DocSnippet> searchWithScope(const std::string& query, int top_k,
                                            const std::string& scope) const;
    /// Substring match on path/title/excerpt (exact identifier lookup); bypasses TF-IDF threshold.
    std::vector<DocSnippet> searchLiteral(const std::string& query, int top_k,
                                          const std::string& scope = "docs") const;
    bool empty() const { return m_docs.empty(); }
    /// Compare on-disk mtimes to manifest; patch changed/removed files (TD-033).
    IndexSyncResult syncFromCatalog(const ILLMKnowledgeCatalog& catalog,
                                  const std::filesystem::path& repository_root,
                                  int max_files = kIndexMaxFiles);
    const std::map<std::string, std::int64_t>& fileMtimes() const { return m_file_mtimes; }

private:
    struct DocRecord {
        std::string source_id;
        std::string path;
        std::string title;
        std::string excerpt;
        LLMContentKind content_kind = LLMContentKind::Doc;
        int start_line = 0;
        std::map<std::string, int> term_freq;
        int length = 0;
        std::vector<float> embedding;
    };

    void indexDocument(DocRecord rec);
    void rebuildDocFreq();
    bool removeByRepoPath(const std::string& repo_relative_path);
    bool indexCatalogFile(const CatalogIndexedFile& file, const std::filesystem::path& repository_root);
    void refreshFileMtimeManifest(const ILLMKnowledgeCatalog& catalog,
                                  const std::filesystem::path& repository_root, int max_files);
    std::vector<DocSnippet> searchInternal(const std::string& query, int top_k,
                                           bool include_doc, bool include_source,
                                           bool include_runtime) const;

    std::vector<DocRecord> m_docs;
    std::map<std::string, int> m_doc_freq;
    int m_doc_count = 0;
    std::map<std::string, std::int64_t> m_file_mtimes;
};

std::vector<DocSnippet> searchDocsWithIndex(const std::vector<std::filesystem::path>& roots,
                                            const std::string& query, int top_k);

} // namespace RDK::LLM

#endif
