#ifndef RDK_ILLM_PROJECT_CONTEXT_PROVIDER_H
#define RDK_ILLM_PROJECT_CONTEXT_PROVIDER_H

#include <filesystem>
#include <string>
#include <vector>

#include "ILLMKnowledgeCatalog.h"

namespace RDK::LLM {

struct LibraryDescriptor {
    std::string library_id;
    std::string cl_desc_folder;
};

struct DocSnippet {
    std::string source_id;
    std::string path;
    std::string title;
    std::string excerpt;
    double score = 0.0;
    LLMContentKind content_kind = LLMContentKind::Doc;
    int start_line = 0;
};

struct ProjectPaths {
    std::filesystem::path repository_root;
    std::filesystem::path bin_root;
    std::filesystem::path cl_desc_root;
    std::filesystem::path configs_root;
    std::filesystem::path docs_root;
};

class ULLMToolRegistry;
class URdkDomainAccess;

class ILLMProjectContextProvider {
public:
    virtual ~ILLMProjectContextProvider() = default;
    virtual ProjectPaths paths() const = 0;
    virtual std::vector<LibraryDescriptor> loadedLibraries() const = 0;
    virtual std::string clDescFragment(const std::string& class_name,
                                       const std::string& locale = "ru-RU") const = 0;
    virtual std::vector<DocSnippet> searchDocs(const std::string& query,
                                               int top_k = 5,
                                               int max_chars = 8000) const = 0;
    virtual void registerExtraTools(ULLMToolRegistry& registry, URdkDomainAccess& domain) { (void)registry; (void)domain; }
};

class ILLMProjectContextProviderRegistry {
public:
    void setPrimary(ILLMProjectContextProvider* provider) { m_primary = provider; }
    ILLMProjectContextProvider* primary() const { return m_primary; }

private:
    ILLMProjectContextProvider* m_primary = nullptr;
};

} // namespace RDK::LLM

#endif
