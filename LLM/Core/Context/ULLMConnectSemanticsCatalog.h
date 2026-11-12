#ifndef RDK_ULLM_CONNECT_SEMANTICS_CATALOG_H
#define RDK_ULLM_CONNECT_SEMANTICS_CATALOG_H

#include "ULinkPatternCatalog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace RDK::LLM {

struct ConnectSemanticEntry {
    std::string from_class;
    std::string to_class;
    std::string from_port;
    std::string to_port;
    std::string from_role;
    std::string to_role;
    std::vector<std::string> labels_ru;
    std::vector<std::string> labels_en;
    int count = 0;
    double confidence = 0.0;
};

class ULLMConnectSemanticsCatalog {
public:
    bool loadFromFile(const std::filesystem::path& json_path);
    bool empty() const { return m_entries.empty(); }

    struct Suggestion {
        std::string from_port;
        std::string to_port;
        double score = 0.0;
        std::string rationale;
    };

    std::vector<Suggestion> suggestContainerPair(const std::string& from_class,
                                                 const std::string& to_class,
                                                 int top_k = 3) const;

    bool matchGoalPortHint(const std::string& goal_lower, std::string& out_from_port,
                           std::string& out_to_port) const;

private:
    std::vector<ConnectSemanticEntry> m_entries;
};

std::vector<ConnectSemanticEntry> buildConnectSemanticsFromConfigs(
    const std::filesystem::path& configs_root);
std::vector<ConnectSemanticEntry> buildConnectSemanticsFromLinkPatterns(
    const std::vector<LinkPatternItem>& patterns);

bool writeConnectSemantics(const std::filesystem::path& out_json,
                           const std::filesystem::path& manifest_json,
                           const std::vector<ConnectSemanticEntry>& entries,
                           const std::string& fingerprint, int configs_scanned);

const ULLMConnectSemanticsCatalog& defaultConnectSemanticsCatalog();

} // namespace RDK::LLM

#endif
