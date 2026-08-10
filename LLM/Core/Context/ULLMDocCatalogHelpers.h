#ifndef RDK_ULLM_DOC_CATALOG_HELPERS_H
#define RDK_ULLM_DOC_CATALOG_HELPERS_H

#include "../LlmTypes.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace RDK::LLM {

/// DD-DOC-002: catalog inventory / doc-link enforcement helpers.

using DocLinkItem = TurnDocLinkView;

/// RU/EN heuristic: «какие / доступны / список / модели / классы / available…».
bool isCatalogInventoryQuery(const std::string& text);

/// User explicitly asked for documentation links.
bool userAskedForDocumentationLinks(const std::string& text);

/// Library / project docs search and component-class list tools (pattern + known names).
bool isLibraryDocsOrCatalogReadTool(const std::string& tool_name);

bool proseContainsNmsdkDocScheme(const std::string& prose);

void extractDocLinksFromToolResult(const nlohmann::json& result, std::vector<DocLinkItem>& out);

std::vector<DocLinkItem> collectDocLinksFromToolTrace(
    const std::vector<TurnToolInvocationView>& trace);

std::string formatDocLinkFooterMarkdown(const std::vector<DocLinkItem>& links);

/// When user asked for links, prose lacks nmsdk-* schemes, and tool snippets have doc_uri —
/// append a deterministic markdown link list. Returns true if prose was modified.
bool appendDocLinkFooterFromToolTrace(std::string& prose, const std::string& user_text,
                                      const std::vector<TurnToolInvocationView>& trace);

/// Act-or-Clarify recovery body for inventory Query/Explain.
std::string inventoryCatalogRecoveryHint(bool links_requested);

/// Ephemeral Core hint: enumerate via list_* first (not a single-class essay).
std::string inventoryCatalogEphemeralHint();

} // namespace RDK::LLM

#endif
