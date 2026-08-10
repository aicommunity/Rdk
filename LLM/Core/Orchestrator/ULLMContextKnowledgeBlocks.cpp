#include "ULLMContextKnowledgeBlocks.h"

#include "../Context/UDocSearchIndex.h"
#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "../Context/ULLMDocOpenPolicy.h"
#include "../Context/ULinkPatternCatalog.h"

#include <sstream>

namespace RDK::LLM {

namespace {

constexpr std::size_t kDefaultExcerptChars = 400;

void truncateInPlace(std::string& s, const std::size_t max_chars)
{
    if(s.size() <= max_chars)
        return;
    s.resize(max_chars);
    s += "\n...(truncated)";
}

} // namespace

std::string buildLinkPatternHintBlock(const ULinkPatternCatalog& cat,
                                    const std::string& from_class, const std::string& to_class,
                                    int top_k)
{
    if(from_class.empty() || to_class.empty() || cat.empty())
        return {};

    const std::vector<LinkPortSuggestion> suggestions = cat.suggest(from_class, to_class, top_k);
    if(suggestions.empty())
        return {};

    std::ostringstream oss;
    oss << "## Link patterns (index)\n"
        << "- from_class: " << from_class << "\n"
        << "- to_class: " << to_class << "\n";
    for(const LinkPortSuggestion& s : suggestions)
    {
        oss << "- " << s.from_port << " -> " << s.to_port << " (score=" << s.score
            << ", count=" << s.count << ")\n";
    }
    return oss.str();
}

std::string buildConnectSemanticsHintBlock(const ULLMConnectSemanticsCatalog& sem,
                                           const std::string& from_class,
                                           const std::string& to_class, int top_k)
{
    if(from_class.empty() || to_class.empty() || sem.empty())
        return {};

    const auto suggestions = sem.suggestContainerPair(from_class, to_class, top_k);
    if(suggestions.empty())
        return {};

    std::ostringstream oss;
    oss << "## Connect semantics (index)\n"
        << "- from_class: " << from_class << "\n"
        << "- to_class: " << to_class << "\n";
    for(const ULLMConnectSemanticsCatalog::Suggestion& s : suggestions)
    {
        oss << "- " << s.from_port << " -> " << s.to_port << " (score=" << s.score << ")\n";
    }
    oss << "Use get_component_properties if port names are unclear.\n";
    return oss.str();
}

std::string buildConnectInspectHintBlock()
{
    return "## Connect inspect (live wiring)\n"
           "- Named components in connect requests are **subtree anchors**: links often go "
           "between nested published ports under those names, not only root↔root.\n"
           "- Before inventing topology, call `list_model_links` with "
           "`component_long_name` / `from_long_name` / `to_long_name` (subtree match).\n"
           "- For “same as connected to X”, list links involving X (and the hub), then "
           "`connect_components` with the same port names; rewrite nested long_names under "
           "each target root.\n"
           "- Use `get_component_ports` (include_nested=true) for published in/out under a "
           "container.\n";
}

std::string buildDocsPrefetchBlock(UDocSearchIndex& index, const std::string& query,
                                   const std::string& scope, int top_k, std::size_t max_chars,
                                   const std::filesystem::path& repo_root)
{
    if(query.empty() || top_k <= 0)
        return {};

    const std::vector<DocSnippet> hits = index.searchWithScope(query, top_k, scope);
    if(hits.empty())
        return {};

    std::ostringstream prefetch;
    prefetch << "## Prefetched documentation\n";
    for(const DocSnippet& snip : hits)
    {
        std::string excerpt = snip.excerpt;
        if(excerpt.size() > kDefaultExcerptChars)
            excerpt.resize(kDefaultExcerptChars);
        std::string doc_uri;
        if(!repo_root.empty() && !snip.path.empty())
        {
            const std::string rel = repoRelativePosixPath(snip.path, repo_root);
            if(!rel.empty())
                doc_uri = makeDocUriFromRepoRelative(rel);
        }
        prefetch << "- [" << snip.source_id << "] ";
        if(!doc_uri.empty())
        {
            const std::string label = snip.title.empty() ? snip.path : snip.title;
            prefetch << "[" << label << "](" << doc_uri << ") doc_uri=" << doc_uri << ": ";
        }
        else
        {
            prefetch << snip.path << ": ";
        }
        prefetch << excerpt << "\n";
    }
    std::string block = prefetch.str();
    truncateInPlace(block, max_chars);
    return block;
}

} // namespace RDK::LLM
