#include "UDocSearchCatalog.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

bool globMatch(const std::string& text, const std::string& pattern)
{
    std::size_t ti = 0;
    std::size_t pi = 0;
    std::size_t star = std::string::npos;
    std::size_t match = 0;
    while(ti < text.size())
    {
        if(pi < pattern.size()
           && (pattern[pi] == '?' || pattern[pi] == text[ti]))
        {
            ++ti;
            ++pi;
            continue;
        }
        if(pi < pattern.size() && pattern[pi] == '*')
        {
            star = pi++;
            match = ti;
            continue;
        }
        if(star != std::string::npos)
        {
            pi = star + 1;
            ti = ++match;
            continue;
        }
        return false;
    }
    while(pi < pattern.size() && pattern[pi] == '*')
        ++pi;
    return pi == pattern.size();
}

std::string normalizeSlashes(std::string path)
{
    for(char& c : path)
    {
        if(c == '\\')
            c = '/';
    }
    return path;
}

} // namespace

LLMContentKind contentKindForSource(const LLMKnowledgeSource& source)
{
    for(const std::string& ext : source.extensions)
    {
        if(ext == ".md" || ext == ".txt")
            return LLMContentKind::Doc;
        if(ext == ".xml" || ext == ".ini")
            return LLMContentKind::RuntimeXml;
    }
    return LLMContentKind::Source;
}

bool shouldExcludePath(const fs::path& repo_root, const fs::path& file_path,
                       const std::vector<std::string>& source_excludes)
{
    const std::string rel = normalizeSlashes(fs::relative(file_path, repo_root).string());
    static const char* kGlobalExcludes[] = {
        "Rdk/ThirdParty/**",
        "Rdk/Docs.old/**",
        "Docs.old/**",
        "Rdk/GUI/Win/**",
        "Rdk/GUI/BCB/**",
        "Rdk/GUI/MSVS/**",
        "Rdk/GUI/OpenCV/**",
        "**/build/**",
        "**/build-*/**",
        "**/_deps/**",
        "**/*_autogen/**",
        "Tests/**",
        "Rdk/Tests/**",
        "**/.build_*/**",
        nullptr};
    auto check = [&](const std::string& pattern) {
        return globMatch(rel, pattern);
    };
    for(const char** p = kGlobalExcludes; *p; ++p)
    {
        if(check(*p))
            return true;
    }
    for(const std::string& pattern : source_excludes)
    {
        if(check(pattern))
            return true;
    }
    return false;
}

bool extensionMatches(const fs::path& file_path, const std::vector<std::string>& extensions)
{
    const std::string ext = file_path.extension().string();
    return std::any_of(extensions.begin(), extensions.end(),
                       [&](const std::string& candidate) { return candidate == ext; });
}

std::string makeRepoRelativePath(const fs::path& repo_root, const fs::path& absolute_path)
{
    std::error_code ec;
    const fs::path rel = fs::relative(absolute_path, repo_root, ec);
    if(ec)
        return normalizeSlashes(absolute_path.string());
    return normalizeSlashes(rel.string());
}

std::string extractMarkdownTitle(const std::string& content, const std::string& fallback)
{
    std::istringstream iss(content);
    std::string line;
    while(std::getline(iss, line))
    {
        std::size_t pos = line.find('#');
        if(pos == std::string::npos)
            continue;
        while(pos < line.size() && line[pos] == '#')
            ++pos;
        while(pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])))
            ++pos;
        if(pos < line.size())
            return line.substr(pos);
    }
    return fallback;
}

std::string readSourceExcerpt(const fs::path& file_path, const int max_lines, int& start_line_out)
{
    start_line_out = 1;
    std::ifstream in(file_path);
    if(!in)
        return {};
    std::ostringstream oss;
    std::string line;
    int line_no = 0;
    while(line_no < max_lines && std::getline(in, line))
    {
        ++line_no;
        oss << line << '\n';
    }
    return oss.str();
}

} // namespace RDK::LLM
