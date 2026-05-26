#ifndef RDK_UDOC_SEARCH_CATALOG_H
#define RDK_UDOC_SEARCH_CATALOG_H

#include "ILLMKnowledgeCatalog.h"

namespace RDK::LLM {

bool shouldExcludePath(const std::filesystem::path& repo_root,
                       const std::filesystem::path& file_path,
                       const std::vector<std::string>& source_excludes);

bool extensionMatches(const std::filesystem::path& file_path,
                      const std::vector<std::string>& extensions);

std::string makeRepoRelativePath(const std::filesystem::path& repo_root,
                                 const std::filesystem::path& absolute_path);

std::string extractMarkdownTitle(const std::string& content, const std::string& fallback);

std::string readSourceExcerpt(const std::filesystem::path& file_path, int max_lines,
                              int& start_line_out);

struct CatalogIndexedFile {
    std::filesystem::path absolute_path;
    std::string repo_relative_path;
    std::string source_id;
    LLMContentKind content_kind = LLMContentKind::Doc;
    std::int64_t mtime_unix_sec = 0;
};

std::int64_t fileMtimeUnixSec(const std::filesystem::path& file_path);

std::vector<CatalogIndexedFile> enumerateCatalogFiles(const ILLMKnowledgeCatalog& catalog,
                                                      const std::filesystem::path& repository_root,
                                                      int max_files);

} // namespace RDK::LLM

#endif
