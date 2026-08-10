#include "ULLMDocCatalogHelpers.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool containsInsensitiveAscii(const std::string& haystack_lower, const char* needle)
{
    return haystack_lower.find(needle) != std::string::npos;
}

bool containsRaw(const std::string& haystack, const char* needle)
{
    return haystack.find(needle) != std::string::npos;
}

void pushUniqueLink(std::vector<DocLinkItem>& out, const std::string& title,
                    const std::string& uri)
{
    if(uri.empty())
        return;
    for(const DocLinkItem& existing : out)
    {
        if(existing.uri == uri)
            return;
    }
    DocLinkItem item;
    item.title = title.empty() ? uri : title;
    item.uri = uri;
    out.push_back(std::move(item));
}

} // namespace

bool isCatalogInventoryQuery(const std::string& text)
{
    if(text.empty())
        return false;
    const std::string lower = toLowerAscii(text);
    return containsRaw(text, "какие") || containsRaw(text, "доступн")
           || containsRaw(text, "список") || containsRaw(text, "модел")
           || containsRaw(text, "класс") || containsInsensitiveAscii(lower, "neuron model")
           || containsInsensitiveAscii(lower, "available")
           || containsInsensitiveAscii(lower, "what models")
           || containsInsensitiveAscii(lower, "list of")
           || containsInsensitiveAscii(lower, "classes");
}

bool userAskedForDocumentationLinks(const std::string& text)
{
    if(text.empty())
        return false;
    const std::string lower = toLowerAscii(text);
    return containsRaw(text, "ссылк") || containsRaw(text, "документац")
           || containsInsensitiveAscii(lower, "doc_uri")
           || containsInsensitiveAscii(lower, "documentation")
           || containsInsensitiveAscii(lower, "links")
           || containsInsensitiveAscii(lower, "docs link");
}

bool isLibraryDocsOrCatalogReadTool(const std::string& tool_name)
{
    if(tool_name == "search_project_docs" || tool_name == "list_registered_classes"
       || tool_name == "list_help_topics" || tool_name == "describe_class")
        return true;

    constexpr const char kSearchPrefix[] = "search_";
    constexpr const char kDocsSuffix[] = "_docs";
    if(tool_name.size() > sizeof(kSearchPrefix) - 1 + sizeof(kDocsSuffix) - 1
       && tool_name.compare(0, sizeof(kSearchPrefix) - 1, kSearchPrefix) == 0
       && tool_name.compare(tool_name.size() - (sizeof(kDocsSuffix) - 1), sizeof(kDocsSuffix) - 1,
                            kDocsSuffix)
              == 0)
        return true;

    constexpr const char kListPrefix[] = "list_";
    constexpr const char kClassesSuffix[] = "_component_classes";
    if(tool_name.size() > sizeof(kListPrefix) - 1 + sizeof(kClassesSuffix) - 1
       && tool_name.compare(0, sizeof(kListPrefix) - 1, kListPrefix) == 0
       && tool_name.compare(tool_name.size() - (sizeof(kClassesSuffix) - 1),
                            sizeof(kClassesSuffix) - 1, kClassesSuffix)
              == 0)
        return true;

    return false;
}

bool proseContainsNmsdkDocScheme(const std::string& prose)
{
    return prose.find("nmsdk-doc:") != std::string::npos
           || prose.find("nmsdk-help:") != std::string::npos
           || prose.find("nmsdk-class:") != std::string::npos;
}

void extractDocLinksFromToolResult(const nlohmann::json& result, std::vector<DocLinkItem>& out)
{
    if(!result.is_object())
        return;

    if(result.contains("snippets") && result["snippets"].is_array())
    {
        for(const auto& snip : result["snippets"])
        {
            if(!snip.is_object())
                continue;
            const std::string uri = snip.value("doc_uri", "");
            if(uri.empty())
                continue;
            std::string title = snip.value("title", "");
            if(title.empty())
                title = snip.value("path", "");
            pushUniqueLink(out, title, uri);
        }
    }

    if(result.contains("doc_uri") && result["doc_uri"].is_string())
    {
        const std::string uri = result["doc_uri"].get<std::string>();
        std::string title = result.value("title", "");
        if(title.empty())
            title = result.value("path", "");
        if(title.empty() && result.contains("help_topic") && result["help_topic"].is_string())
            title = result["help_topic"].get<std::string>();
        if(title.empty() && result.contains("class_name") && result["class_name"].is_string())
            title = result["class_name"].get<std::string>();
        pushUniqueLink(out, title, uri);
    }
}

std::vector<DocLinkItem> collectDocLinksFromToolTrace(
    const std::vector<TurnToolInvocationView>& trace)
{
    std::vector<DocLinkItem> out;
    for(const TurnToolInvocationView& inv : trace)
    {
        if(!inv.ok)
            continue;
        for(const DocLinkItem& link : inv.doc_links)
            pushUniqueLink(out, link.title, link.uri);
    }
    return out;
}

std::string formatDocLinkFooterMarkdown(const std::vector<DocLinkItem>& links)
{
    if(links.empty())
        return {};
    std::string body = "\n\n## Documentation links\n";
    for(const DocLinkItem& link : links)
        body += "- [" + link.title + "](" + link.uri + ")\n";
    return body;
}

bool appendDocLinkFooterFromToolTrace(std::string& prose, const std::string& user_text,
                                      const std::vector<TurnToolInvocationView>& trace)
{
    if(!userAskedForDocumentationLinks(user_text))
        return false;
    if(proseContainsNmsdkDocScheme(prose))
        return false;
    const std::vector<DocLinkItem> links = collectDocLinksFromToolTrace(trace);
    if(links.empty())
        return false;
    prose += formatDocLinkFooterMarkdown(links);
    return true;
}

std::string inventoryCatalogRecoveryHint(bool links_requested)
{
    std::string hint =
        "Catalog / inventory request: first call list_pulse_component_classes "
        "(or list_registered_classes with library_filter for Pulse) to enumerate classes — "
        "do not write a single-component essay. Then search_*_docs / search_project_docs "
        "using class or family names (CSNM, iaF, Izhikevich, cable). "
        "Prefer get_net_snapshot only for live graph questions.";
    if(links_requested)
    {
        hint += " User asked for documentation links: final answer MUST include markdown "
                "[title](doc_uri) using snippet.doc_uri (nmsdk-doc:...) from tool results.";
    }
    hint += " If a prior tool already returned ok catalog/docs data, write the list now. "
            "Use NO_SUITABLE_TOOL only if list and docs tools fail.";
    return hint;
}

std::string inventoryCatalogEphemeralHint()
{
    return "## Catalog inventory\n"
           "- Do not write a single-component essay.\n"
           "- Enumerate classes from `list_*_component_classes` / `list_registered_classes` first, "
           "then cite docs via `search_*_docs` with markdown `doc_uri` links when asked.\n";
}

} // namespace RDK::LLM
