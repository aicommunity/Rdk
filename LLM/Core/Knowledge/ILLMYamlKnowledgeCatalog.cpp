#include "ILLMYamlKnowledgeCatalog.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace RDK::LLM {

namespace {

std::string trim(const std::string& s)
{
    std::size_t a = 0;
    while(a < s.size() && std::isspace(static_cast<unsigned char>(s[a])))
        ++a;
    std::size_t b = s.size();
    while(b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
        --b;
    return s.substr(a, b - a);
}

std::string unquote(std::string s)
{
    s = trim(std::move(s));
    if(s.size() >= 2)
    {
        const char q = s.front();
        if((q == '"' || q == '\'') && s.back() == q)
            return s.substr(1, s.size() - 2);
    }
    return s;
}

std::vector<std::string> parseInlineList(const std::string& raw)
{
    std::vector<std::string> out;
    std::string s = trim(raw);
    if(s.size() < 2 || s.front() != '[' || s.back() != ']')
        return out;
    s = s.substr(1, s.size() - 2);
    std::string cur;
    bool in_quotes = false;
    char quote = 0;
    for(char c : s)
    {
        if((c == '"' || c == '\'') && (!in_quotes || c == quote))
        {
            if(!in_quotes)
            {
                in_quotes = true;
                quote = c;
            }
            else
            {
                in_quotes = false;
                quote = 0;
            }
            cur += c;
            continue;
        }
        if(c == ',' && !in_quotes)
        {
            const std::string token = unquote(cur);
            if(!token.empty())
                out.push_back(token);
            cur.clear();
            continue;
        }
        cur += c;
    }
    const std::string token = unquote(cur);
    if(!token.empty())
        out.push_back(token);
    return out;
}

std::optional<LLMKnowledgeKind> parseKind(const std::string& raw)
{
    const std::string v = trim(raw);
    if(v == "ProductDocs")
        return LLMKnowledgeKind::ProductDocs;
    if(v == "LibraryDocs")
        return LLMKnowledgeKind::LibraryDocs;
    if(v == "RdkCoreDocs")
        return LLMKnowledgeKind::RdkCoreDocs;
    if(v == "BinDocs")
        return LLMKnowledgeKind::BinDocs;
    if(v == "RdkSource")
        return LLMKnowledgeKind::RdkSource;
    if(v == "LibrarySource")
        return LLMKnowledgeKind::LibrarySource;
    if(v == "AppSource")
        return LLMKnowledgeKind::AppSource;
    return std::nullopt;
}

std::size_t indentation(const std::string& line)
{
    std::size_t n = 0;
    while(n < line.size() && std::isspace(static_cast<unsigned char>(line[n])))
        ++n;
    return n;
}

std::string makeFingerprint(const std::string& s)
{
    const std::size_t h = std::hash<std::string>{}(s);
    std::ostringstream oss;
    oss << std::hex << h;
    std::string out = oss.str();
    if(out.size() > 16)
        out.resize(16);
    return out;
}

} // namespace

std::optional<UYamlKnowledgeCatalog> UYamlKnowledgeCatalog::load(
    const std::filesystem::path& manifest_path, const std::filesystem::path& repository_root,
    std::string* error_out)
{
    if(error_out)
        error_out->clear();
    std::ifstream in(manifest_path);
    if(!in)
    {
        if(error_out)
            *error_out = "cannot open manifest: " + manifest_path.string();
        return std::nullopt;
    }

    UYamlKnowledgeCatalog catalog;
    catalog.m_manifest_path = manifest_path;

    enum class Mode { None, Sources, Prebuilt };
    Mode mode = Mode::None;
    std::optional<LLMKnowledgeSource> current;
    std::string pending_list_field;
    std::size_t pending_indent = 0;

    auto flushCurrent = [&]() {
        if(!current)
            return;
        if(current->source_id.empty() || current->root.empty() || current->extensions.empty())
        {
            current.reset();
            return;
        }
        std::error_code ec;
        std::filesystem::path resolved = current->root;
        if(resolved.is_relative())
            resolved = std::filesystem::weakly_canonical(repository_root / resolved, ec);
        if(ec)
            resolved = repository_root / current->root;
        current->root = resolved;
        catalog.m_sources.push_back(*current);
        current.reset();
    };

    std::string line;
    while(std::getline(in, line))
    {
        const std::string raw = line;
        const std::string t = trim(raw);
        if(t.empty() || t.rfind("#", 0) == 0)
            continue;

        const std::size_t ind = indentation(raw);
        if(!pending_list_field.empty()
           && !(t.rfind("- ", 0) == 0 && ind > pending_indent))
            pending_list_field.clear();

        if(t == "sources:")
        {
            flushCurrent();
            mode = Mode::Sources;
            continue;
        }
        if(t.rfind("prebuilt_index_directory:", 0) == 0)
        {
            flushCurrent();
            mode = Mode::Prebuilt;
            const std::string val = trim(t.substr(std::string("prebuilt_index_directory:").size()));
            if(!val.empty())
                catalog.m_prebuilt_index_dir = unquote(val);
            continue;
        }

        if(mode == Mode::Sources)
        {
            if(!pending_list_field.empty() && t.rfind("- ", 0) == 0)
            {
                if(!current)
                    continue;
                const std::string item = unquote(t.substr(2));
                if(item.empty())
                    continue;
                if(pending_list_field == "extensions")
                    current->extensions.push_back(item);
                else if(pending_list_field == "exclude_globs")
                    current->exclude_globs.push_back(item);
                else if(pending_list_field == "include_files_only")
                    current->include_files_only.push_back(item);
                continue;
            }

            if(t.rfind("- ", 0) == 0)
            {
                flushCurrent();
                current = LLMKnowledgeSource{};
                const std::string rest = trim(t.substr(2));
                if(!rest.empty())
                {
                    const std::size_t pos = rest.find(':');
                    if(pos != std::string::npos)
                    {
                        const std::string key = trim(rest.substr(0, pos));
                        const std::string value = unquote(rest.substr(pos + 1));
                        if(key == "source_id")
                            current->source_id = value;
                    }
                }
                continue;
            }
            if(!current)
                continue;

            const std::size_t pos = t.find(':');
            if(pos == std::string::npos)
                continue;
            const std::string key = trim(t.substr(0, pos));
            const std::string raw_value = trim(t.substr(pos + 1));
            const std::string value = unquote(raw_value);
            if(key == "source_id")
                current->source_id = value;
            else if(key == "root")
                current->root = value;
            else if(key == "kind")
            {
                if(const auto kind = parseKind(value))
                    current->kind = *kind;
            }
            else if(key == "extensions" || key == "exclude_globs" || key == "include_files_only")
            {
                std::vector<std::string> items = parseInlineList(raw_value);
                if(items.empty() && raw_value.empty())
                {
                    pending_list_field = key;
                    pending_indent = ind;
                    continue;
                }
                if(key == "extensions")
                    current->extensions.insert(current->extensions.end(), items.begin(), items.end());
                else if(key == "exclude_globs")
                    current->exclude_globs.insert(current->exclude_globs.end(), items.begin(),
                                                  items.end());
                else
                    current->include_files_only.insert(current->include_files_only.end(), items.begin(),
                                                       items.end());
            }
            continue;
        }

        if(mode == Mode::Prebuilt && !catalog.m_prebuilt_index_dir.empty())
            continue;
    }

    flushCurrent();
    if(catalog.m_prebuilt_index_dir.empty())
        catalog.m_prebuilt_index_dir = repository_root / "Bin/LLM/index";
    else if(catalog.m_prebuilt_index_dir.is_relative())
        catalog.m_prebuilt_index_dir = repository_root / catalog.m_prebuilt_index_dir;

    if(catalog.m_sources.empty())
    {
        if(error_out)
            *error_out = "manifest has no valid sources";
        return std::nullopt;
    }
    return catalog;
}

std::string UYamlKnowledgeCatalog::catalogFingerprint() const
{
    std::ostringstream oss;
    oss << m_manifest_path.string() << '\n';
    for(const LLMKnowledgeSource& s : m_sources)
    {
        oss << s.source_id << '|' << s.root.string() << '|'
            << static_cast<int>(s.kind) << '\n';
    }
    return makeFingerprint(oss.str());
}

} // namespace RDK::LLM
