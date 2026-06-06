#include "UDocSearchCatalog.h"

#include <algorithm>
#include <chrono>
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

std::int64_t fileMtimeUnixSec(const fs::path& file_path)
{
    std::error_code ec;
    const auto mtime = fs::last_write_time(file_path, ec);
    if(ec)
        return 0;
    return std::chrono::duration_cast<std::chrono::seconds>(mtime.time_since_epoch()).count();
}

std::vector<CatalogIndexedFile> enumerateCatalogFiles(const ILLMKnowledgeCatalog& catalog,
                                                      const fs::path& repository_root,
                                                      const int max_files)
{
    std::vector<CatalogIndexedFile> out;
    out.reserve(static_cast<size_t>(max_files));

    for(const LLMKnowledgeSource& source : catalog.sources())
    {
        if(!fs::exists(source.root) || static_cast<int>(out.size()) >= max_files)
            continue;

        auto try_add = [&](const fs::path& file) {
            if(static_cast<int>(out.size()) >= max_files)
                return;
            if(!fs::is_regular_file(file) || !extensionMatches(file, source.extensions))
                return;
            if(shouldExcludePath(repository_root, file, source.exclude_globs))
                return;
            CatalogIndexedFile entry;
            entry.absolute_path = file;
            entry.repo_relative_path = makeRepoRelativePath(repository_root, file);
            entry.source_id = source.source_id;
            entry.content_kind = contentKindForSource(source);
            entry.mtime_unix_sec = fileMtimeUnixSec(file);
            out.push_back(std::move(entry));
        };

        if(!source.include_files_only.empty())
        {
            for(const std::string& rel : source.include_files_only)
                try_add(source.root / rel);
            continue;
        }

        std::error_code ec;
        for(fs::recursive_directory_iterator it(source.root, ec), end;
            it != end && static_cast<int>(out.size()) < max_files; it.increment(ec))
        {
            if(ec)
                break;
            if(it->is_regular_file())
                try_add(it->path());
        }
    }
    return out;
}

} // namespace RDK::LLM
