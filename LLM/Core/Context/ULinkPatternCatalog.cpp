#include "ULinkPatternCatalog.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <regex>
#include <set>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

namespace {

struct RawLinkRow {
    std::string from_short;
    std::string to_short;
    std::string from_port;
    std::string to_port;
    std::string item_side_name;
    std::string connector_side_name;
};

std::string readFileText(const std::filesystem::path& path)
{
    std::ifstream in(path);
    if(!in)
        return {};
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

std::string trim(const std::string& s)
{
    const auto is_ws = [](unsigned char c) { return std::isspace(c) != 0; };
    auto b = std::find_if_not(s.begin(), s.end(), is_ws);
    auto e = std::find_if_not(s.rbegin(), s.rend(), is_ws).base();
    if(b >= e)
        return {};
    return std::string(b, e);
}

std::string rootComponent(const std::string& path)
{
    const auto dot = path.find('.');
    return dot == std::string::npos ? path : path.substr(0, dot);
}

std::string portWithoutRoot(const std::string& path)
{
    const auto dot = path.find('.');
    if(dot == std::string::npos)
        return "Output";
    return path.substr(dot + 1);
}

std::map<std::string, std::string> parseClassMap(const std::filesystem::path& parameters_xml)
{
    std::map<std::string, std::string> out;
    const std::string text = readFileText(parameters_xml);
    if(text.empty())
        return out;

    static const std::regex kComp(
        R"(<\s*([A-Za-z0-9_]+)[^>]*\sClass\s*=\s*\"([^\"]+)\"[^>]*>)");
    for(std::sregex_iterator it(text.begin(), text.end(), kComp), end; it != end; ++it)
        out[it->str(1)] = it->str(2);
    return out;
}

std::vector<RawLinkRow> parseLinks(const std::string& model_text)
{
    std::vector<RawLinkRow> rows;
    // Avoid std::regex on large XML (can stack overflow on some libstdc++ builds).
    const std::string elem_open = "<elem Type=\"ULink\">";
    const std::string elem_close = "</elem>";
    const std::string item_open = "<Item";
    const std::string conn_open = "<Connector";

    auto findAttr = [](const std::string& s, size_t from, const std::string& key) -> std::string {
        const size_t k = s.find(key, from);
        if(k == std::string::npos)
            return {};
        const size_t q1 = s.find('"', k + key.size());
        if(q1 == std::string::npos)
            return {};
        const size_t q2 = s.find('"', q1 + 1);
        if(q2 == std::string::npos)
            return {};
        return s.substr(q1 + 1, q2 - (q1 + 1));
    };
    auto findTagText = [](const std::string& s, size_t tag_start) -> std::string {
        const size_t gt = s.find('>', tag_start);
        if(gt == std::string::npos)
            return {};
        const size_t lt = s.find('<', gt + 1);
        if(lt == std::string::npos || lt <= gt + 1)
            return {};
        return s.substr(gt + 1, lt - (gt + 1));
    };

    size_t pos = 0;
    while(true)
    {
        const size_t e0 = model_text.find(elem_open, pos);
        if(e0 == std::string::npos)
            break;
        const size_t e1 = model_text.find(elem_close, e0);
        if(e1 == std::string::npos)
            break;

        const size_t elem_begin = e0 + elem_open.size();
        const std::string elem = model_text.substr(elem_begin, e1 - elem_begin);

        // Item (single)
        const size_t item_pos = elem.find(item_open);
        if(item_pos == std::string::npos)
        {
            pos = e1 + elem_close.size();
            continue;
        }
        const std::string item_side_name = trim(findAttr(elem, item_pos, "Name="));
        const std::string item_path = trim(findTagText(elem, item_pos));
        if(item_path.empty())
        {
            pos = e1 + elem_close.size();
            continue;
        }
        const std::string from_short = rootComponent(item_path);
        const std::string from_port = portWithoutRoot(item_path);

        // Connectors (0..N)
        size_t cp = 0;
        while(true)
        {
            const size_t cpos = elem.find(conn_open, cp);
            if(cpos == std::string::npos)
                break;
            RawLinkRow row;
            row.item_side_name = item_side_name;
            row.connector_side_name = trim(findAttr(elem, cpos, "Name="));
            const std::string conn_path = trim(findTagText(elem, cpos));
            row.from_short = from_short;
            row.to_short = rootComponent(conn_path);
            row.from_port = from_port;
            row.to_port = portWithoutRoot(conn_path);
            if(!row.to_short.empty() && !row.to_port.empty())
                rows.push_back(std::move(row));
            cp = cpos + conn_open.size();
        }

        pos = e1 + elem_close.size();
    }
    return rows;
}

std::string keyFor(const LinkPatternItem& i)
{
    return i.from_class + "|" + i.to_class + "|" + i.from_port + "|" + i.to_port;
}

} // namespace

bool ULinkPatternCatalog::loadFromFile(const std::filesystem::path& json_path)
{
    m_items.clear();
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

    for(const auto& item_j : j)
    {
        LinkPatternItem item;
        item.from_class = item_j.value("from_class", "");
        item.to_class = item_j.value("to_class", "");
        item.from_port = item_j.value("from_port", "");
        item.to_port = item_j.value("to_port", "");
        item.item_side_name = item_j.value("item_side_name", "");
        item.connector_side_name = item_j.value("connector_side_name", "");
        item.count = item_j.value("count", 0);
        if(item_j.contains("examples") && item_j["examples"].is_array())
        {
            for(const auto& ex : item_j["examples"])
                item.examples.push_back(ex.get<std::string>());
        }
        if(item.from_class.empty() || item.to_class.empty() || item.from_port.empty()
           || item.to_port.empty())
            continue;
        m_items.push_back(std::move(item));
    }
    return !m_items.empty();
}

std::vector<LinkPortSuggestion> ULinkPatternCatalog::suggest(const std::string& from_class,
                                                             const std::string& to_class,
                                                             int top_k) const
{
    std::vector<LinkPortSuggestion> out;
    int max_count = 0;
    for(const LinkPatternItem& i : m_items)
    {
        if(i.from_class == from_class && i.to_class == to_class)
            max_count = std::max(max_count, i.count);
    }
    if(max_count <= 0)
        return out;

    for(const LinkPatternItem& i : m_items)
    {
        if(i.from_class != from_class || i.to_class != to_class)
            continue;
        LinkPortSuggestion s;
        s.from_class = i.from_class;
        s.to_class = i.to_class;
        s.from_port = i.from_port;
        s.to_port = i.to_port;
        s.count = i.count;
        s.score = static_cast<double>(i.count) / static_cast<double>(max_count);
        out.push_back(std::move(s));
    }
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) { return a.score > b.score; });
    if(top_k > 0 && static_cast<int>(out.size()) > top_k)
        out.resize(static_cast<size_t>(top_k));
    return out;
}

std::vector<LinkPatternItem> buildLinkPatternsFromConfigs(const std::filesystem::path& configs_root)
{
    std::map<std::string, LinkPatternItem> agg;
    if(!std::filesystem::exists(configs_root))
        return {};

    for(const auto& entry : std::filesystem::recursive_directory_iterator(configs_root))
    {
        if(!entry.is_regular_file())
            continue;
        const std::string name = entry.path().filename().string();
        if(name.rfind("Model_", 0) != 0 || entry.path().extension() != ".xml")
            continue;

        const std::filesystem::path dir = entry.path().parent_path();
        const auto class_map = parseClassMap(dir / "Parameters_00.xml");
        const auto links = parseLinks(readFileText(entry.path()));
        for(const RawLinkRow& row : links)
        {
            auto from_it = class_map.find(row.from_short);
            auto to_it = class_map.find(row.to_short);
            if(from_it == class_map.end() || to_it == class_map.end())
                continue;
            LinkPatternItem item;
            item.from_class = from_it->second;
            item.to_class = to_it->second;
            item.from_port = row.from_port;
            item.to_port = row.to_port;
            item.item_side_name = row.item_side_name;
            item.connector_side_name = row.connector_side_name;
            const std::string key = keyFor(item);
            auto& slot = agg[key];
            if(slot.from_class.empty())
                slot = item;
            slot.count += 1;
            const std::string ex = dir.generic_string();
            if(std::find(slot.examples.begin(), slot.examples.end(), ex) == slot.examples.end()
               && slot.examples.size() < 3)
                slot.examples.push_back(ex);
        }
    }

    std::vector<LinkPatternItem> out;
    out.reserve(agg.size());
    for(auto& kv : agg)
        out.push_back(std::move(kv.second));
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) { return a.count > b.count; });
    return out;
}

bool writeLinkPatterns(const std::filesystem::path& out_json, const std::filesystem::path& manifest_json,
                       const std::vector<LinkPatternItem>& items, const std::string& fingerprint,
                       int configs_scanned)
{
    std::error_code ec;
    std::filesystem::create_directories(out_json.parent_path(), ec);
    nlohmann::json j = nlohmann::json::array();
    for(const LinkPatternItem& item : items)
    {
        j.push_back({{"from_class", item.from_class},
                     {"to_class", item.to_class},
                     {"from_port", item.from_port},
                     {"to_port", item.to_port},
                     {"item_side_name", item.item_side_name},
                     {"connector_side_name", item.connector_side_name},
                     {"count", item.count},
                     {"examples", item.examples}});
    }
    std::ofstream out(out_json);
    if(!out)
        return false;
    out << j.dump(2);

    nlohmann::json manifest = {{"fingerprint", fingerprint},
                               {"pattern_count", static_cast<int>(items.size())},
                               {"configs_scanned", configs_scanned}};
    std::ofstream mout(manifest_json);
    if(!mout)
        return false;
    mout << manifest.dump(2);
    return true;
}

} // namespace RDK::LLM
