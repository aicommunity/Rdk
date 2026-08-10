#ifndef RDK_ULLM_DOC_OPEN_POLICY_H
#define RDK_ULLM_DOC_OPEN_POLICY_H

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace RDK::LLM {

/// DD-DOC-001: resolve nmsdk-doc / nmsdk-help / nmsdk-class URIs and gate markdown paths.
struct DocOpenResolve {
    bool ok = false;
    std::string error_code; // PATH_NOT_ALLOWED | NOT_FOUND | INVALID_URI
    std::string kind;       // "markdown" | "help" | "class"
    std::filesystem::path abs_path;
    std::string class_name;
    std::string help_topic;
    std::string doc_uri;
};

bool isAllowedMarkdownDocPath(const std::filesystem::path& abs,
                              const std::filesystem::path& repo_root);

/// Prefer posix relative path under repo_root; empty if outside.
std::string repoRelativePosixPath(const std::filesystem::path& abs_or_rel,
                                  const std::filesystem::path& repo_root);

std::string makeDocUriFromRepoRelative(const std::string& rel_posix);
std::string makeHelpUri(const std::string& topic);
std::string makeClassUri(const std::string& class_name);

/// If path resolves under repo_root, set row["doc_uri"].
void enrichSnippetDocUri(nlohmann::json& row, const std::string& path,
                         const std::filesystem::path& repo_root);

DocOpenResolve resolveMarkdownPath(const std::string& path_or_rel,
                                   const std::filesystem::path& repo_root);
DocOpenResolve resolveHelpTopic(const std::string& topic,
                                const std::filesystem::path& repo_root,
                                const std::string& locale = "en");
DocOpenResolve resolveDocUri(const std::string& uri, const std::filesystem::path& repo_root,
                             const std::string& help_locale = "en");

/// Known library Docs folder names under Libraries/.
std::vector<std::string> allowedLibraryDocsRelRoots();

} // namespace RDK::LLM

#endif
