#ifndef RDK_ULLM_LINK_PATTERN_CATALOG_H
#define RDK_ULLM_LINK_PATTERN_CATALOG_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace RDK::LLM {

struct LinkPortSuggestion {
    std::string from_class;
    std::string to_class;
    std::string from_port;
    std::string to_port;
    double score = 0.0;
    int count = 0;
};

struct LinkPatternItem {
    std::string from_class;
    std::string to_class;
    std::string from_port;
    std::string to_port;
    std::string item_side_name;
    std::string connector_side_name;
    int count = 0;
    std::vector<std::string> examples;
};

class ULinkPatternCatalog {
public:
    bool loadFromFile(const std::filesystem::path& json_path);
    bool empty() const { return m_items.empty(); }

    std::vector<LinkPortSuggestion> suggest(const std::string& from_class,
                                            const std::string& to_class,
                                            int top_k = 5) const;

private:
    std::vector<LinkPatternItem> m_items;
};

std::vector<LinkPatternItem> buildLinkPatternsFromConfigs(const std::filesystem::path& configs_root);
bool writeLinkPatterns(const std::filesystem::path& out_json, const std::filesystem::path& manifest_json,
                       const std::vector<LinkPatternItem>& items, const std::string& fingerprint,
                       int configs_scanned);

} // namespace RDK::LLM

#endif
