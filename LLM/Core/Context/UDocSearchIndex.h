#ifndef RDK_UDOC_SEARCH_INDEX_H
#define RDK_UDOC_SEARCH_INDEX_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "ILLMProjectContextProvider.h"

namespace RDK::LLM {

/// TF-IDF + offline random-projection semantic boost (TD-017 hybrid search).
class UDocSearchIndex {
public:
    void build(const std::vector<std::filesystem::path>& roots, int max_files = 800);
    std::vector<DocSnippet> search(const std::string& query, int top_k) const;

private:
    struct DocRecord {
        std::string path;
        std::string title;
        std::string excerpt;
        std::map<std::string, int> term_freq;
        int length = 0;
        std::vector<float> embedding;
    };

    std::vector<DocRecord> m_docs;
    std::map<std::string, int> m_doc_freq;
    int m_doc_count = 0;
};

std::vector<DocSnippet> searchDocsWithIndex(const std::vector<std::filesystem::path>& roots,
                                            const std::string& query, int top_k);

} // namespace RDK::LLM

#endif
