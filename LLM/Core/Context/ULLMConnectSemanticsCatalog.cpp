#include "ULLMConnectSemanticsCatalog.h"

#include "ULinkPatternCatalog.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <mutex>

namespace RDK::LLM {

namespace {

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool looksInternalPort(const std::string& port)
{
    if(port.find('.') != std::string::npos)
        return true;
    const std::string lower = toLower(port);
    return lower.find("ltzone") != std::string::npos || lower.find("excsynapse") != std::string::npos
           || lower.find("soma") != std::string::npos || lower.find("zone") != std::string::npos
           || lower.find("synapse") != std::string::npos;
}

void assignRoles(ConnectSemanticEntry& e)
{
    const std::string fp = toLower(e.from_port);
    const std::string tp = toLower(e.to_port);
    if(fp.find("ltzone") != std::string::npos || fp.find("threshold") != std::string::npos)
    {
        e.from_role = "low_threshold_zone";
        e.labels_en.push_back("LT zone");
        e.labels_ru.push_back("низкопороговая зона");
    }
    if(tp.find("excsynapse") != std::string::npos || tp.find("synapse") != std::string::npos)
    {
        e.to_role = "excitatory_synapse";
        e.labels_en.push_back("excitatory synapse");
        e.labels_ru.push_back("возбуждающий синапс");
    }
}

std::string classPairKey(const std::string& a, const std::string& b)
{
    return a + '\x1e' + b;
}

} // namespace

std::vector<ConnectSemanticEntry> buildConnectSemanticsFromLinkPatterns(
    const std::vector<LinkPatternItem>& patterns)
{
    std::map<std::string, ConnectSemanticEntry> best;
    for(const LinkPatternItem& p : patterns)
    {
        if(!looksInternalPort(p.from_port) && !looksInternalPort(p.to_port))
            continue;
        const std::string key = classPairKey(p.from_class, p.to_class);
        auto it = best.find(key);
        if(it == best.end() || p.count > it->second.count)
        {
            ConnectSemanticEntry e;
            e.from_class = p.from_class;
            e.to_class = p.to_class;
            e.from_port = p.from_port;
            e.to_port = p.to_port;
            e.count = p.count;
            assignRoles(e);
            best[key] = std::move(e);
        }
        else
        {
            it->second.count += p.count;
        }
    }

    std::vector<ConnectSemanticEntry> out;
    out.reserve(best.size());
    for(auto& kv : best)
    {
        int max_in_group = kv.second.count;
        for(const LinkPatternItem& p : patterns)
        {
            if(p.from_class == kv.second.from_class && p.to_class == kv.second.to_class)
                max_in_group = std::max(max_in_group, p.count);
        }
        kv.second.confidence =
            max_in_group > 0 ? static_cast<double>(kv.second.count) / static_cast<double>(max_in_group)
                             : 0.0;
        out.push_back(std::move(kv.second));
    }
    std::sort(out.begin(), out.end(),
              [](const ConnectSemanticEntry& a, const ConnectSemanticEntry& b) {
                  return a.count > b.count;
              });
    return out;
}

std::vector<ConnectSemanticEntry> buildConnectSemanticsFromConfigs(
    const std::filesystem::path& configs_root)
{
    return buildConnectSemanticsFromLinkPatterns(buildLinkPatternsFromConfigs(configs_root));
}

bool writeConnectSemantics(const std::filesystem::path& out_json,
                           const std::filesystem::path& manifest_json,
                           const std::vector<ConnectSemanticEntry>& entries,
                           const std::string& fingerprint, int configs_scanned)
{
    std::error_code ec;
    std::filesystem::create_directories(out_json.parent_path(), ec);
    nlohmann::json j = nlohmann::json::array();
    for(const ConnectSemanticEntry& e : entries)
    {
        j.push_back({{"from_class", e.from_class},
                     {"to_class", e.to_class},
                     {"from_port", e.from_port},
                     {"to_port", e.to_port},
                     {"from_role", e.from_role},
                     {"to_role", e.to_role},
                     {"labels_ru", e.labels_ru},
                     {"labels_en", e.labels_en},
                     {"count", e.count},
                     {"confidence", e.confidence}});
    }
    std::ofstream out(out_json);
    if(!out)
        return false;
    out << j.dump(2);

    nlohmann::json manifest = {{"fingerprint", fingerprint},
                               {"entry_count", static_cast<int>(entries.size())},
                               {"configs_scanned", configs_scanned}};
    std::ofstream mout(manifest_json);
    if(!mout)
        return false;
    mout << manifest.dump(2);
    return true;
}

bool ULLMConnectSemanticsCatalog::loadFromFile(const std::filesystem::path& json_path)
{
    m_entries.clear();
    std::ifstream in(json_path);
    if(!in)
        return false;
    nlohmann::json j;
    try
    {
        in >> j;
    }
    catch(...)
    {
        return false;
    }
    if(!j.is_array())
        return false;
    for(const auto& item : j)
    {
        ConnectSemanticEntry e;
        e.from_class = item.value("from_class", "");
        e.to_class = item.value("to_class", "");
        e.from_port = item.value("from_port", "");
        e.to_port = item.value("to_port", "");
        e.from_role = item.value("from_role", "");
        e.to_role = item.value("to_role", "");
        e.count = item.value("count", 0);
        e.confidence = item.value("confidence", 0.0);
        if(item.contains("labels_ru") && item["labels_ru"].is_array())
            for(const auto& x : item["labels_ru"])
                if(x.is_string())
                    e.labels_ru.push_back(x.get<std::string>());
        if(item.contains("labels_en") && item["labels_en"].is_array())
            for(const auto& x : item["labels_en"])
                if(x.is_string())
                    e.labels_en.push_back(x.get<std::string>());
        if(e.from_class.empty() || e.to_class.empty() || e.from_port.empty() || e.to_port.empty())
            continue;
        m_entries.push_back(std::move(e));
    }
    return !m_entries.empty();
}

std::vector<ULLMConnectSemanticsCatalog::Suggestion> ULLMConnectSemanticsCatalog::suggestContainerPair(
    const std::string& from_class, const std::string& to_class, int top_k) const
{
    std::vector<Suggestion> out;
    int max_count = 0;
    for(const ConnectSemanticEntry& e : m_entries)
    {
        if(e.from_class == from_class && e.to_class == to_class)
            max_count = std::max(max_count, e.count);
    }
    if(max_count <= 0)
        return out;

    for(const ConnectSemanticEntry& e : m_entries)
    {
        if(e.from_class != from_class || e.to_class != to_class)
            continue;
        Suggestion s;
        s.from_port = e.from_port;
        s.to_port = e.to_port;
        s.score = max_count > 0 ? static_cast<double>(e.count) / static_cast<double>(max_count) : 0.0;
        s.rationale = "connect-semantics:" + e.from_class + "->" + e.to_class;
        out.push_back(std::move(s));
    }
    std::sort(out.begin(), out.end(),
              [](const Suggestion& a, const Suggestion& b) { return a.score > b.score; });
    if(top_k > 0 && static_cast<int>(out.size()) > top_k)
        out.resize(static_cast<size_t>(top_k));
    return out;
}

bool ULLMConnectSemanticsCatalog::matchGoalPortHint(const std::string& goal_lower,
                                                    std::string& out_from_port,
                                                    std::string& out_to_port) const
{
    const std::string g = toLower(goal_lower);
    for(const ConnectSemanticEntry& e : m_entries)
    {
        for(const std::string& label : e.labels_ru)
        {
            if(!label.empty() && g.find(toLower(label)) != std::string::npos)
            {
                out_from_port = e.from_port;
                out_to_port = e.to_port;
                return true;
            }
        }
        for(const std::string& label : e.labels_en)
        {
            if(!label.empty() && g.find(toLower(label)) != std::string::npos)
            {
                out_from_port = e.from_port;
                out_to_port = e.to_port;
                return true;
            }
        }
        if(g.find(toLower(e.from_port)) != std::string::npos)
            out_from_port = e.from_port;
        if(g.find(toLower(e.to_port)) != std::string::npos)
            out_to_port = e.to_port;
    }
    return !out_from_port.empty() && !out_to_port.empty();
}

const ULLMConnectSemanticsCatalog& defaultConnectSemanticsCatalog()
{
    static std::once_flag once;
    static ULLMConnectSemanticsCatalog catalog;
    std::call_once(once, []() {
        std::filesystem::path root = std::filesystem::current_path();
        for(int i = 0; i < 8 && root.has_parent_path(); ++i)
        {
            if(std::filesystem::exists(root / "CMakeLists.txt"))
                break;
            root = root.parent_path();
        }
        if(!root.empty())
            catalog.loadFromFile(root / "Bin/LLM/index/connect-semantics.json");
    });
    return catalog;
}

} // namespace RDK::LLM
