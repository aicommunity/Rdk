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

} // namespace RDK::LLM

#endif
