#include "ULLMDocOpenPolicy.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {
namespace {

std::string toPosix(std::filesystem::path p)
{
    p = p.lexically_normal();
    std::string s = p.generic_string();
    while(!s.empty() && (s[0] == '/' || s[0] == '\\'))
        s.erase(s.begin());
    return s;
}

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool startsWithPosix(const std::string& path, const std::string& prefix)
{
    if(path.size() < prefix.size())
        return false;
    if(path.compare(0, prefix.size(), prefix) != 0)
        return false;
    return path.size() == prefix.size() || path[prefix.size()] == '/';
}

bool isSafeHelpTopic(const std::string& topic)
{
    if(topic.empty())
        return true;
    if(topic.find("..") != std::string::npos)
        return false;
    if(topic.find('/') != std::string::npos || topic.find('\\') != std::string::npos)
        return false;
    return true;
}

} // namespace

std::vector<std::string> allowedLibraryDocsRelRoots()
{
    return {"Libraries/Rdk-BasicLib/Docs",         "Libraries/Rdk-CvBasicLib/Docs",
            "Libraries/Rdk-HardwareLib/Docs",      "Libraries/Nmsdk-PulseLib/Docs",
            "Libraries/Nmsdk-MotionControlLib/Docs"};
}

bool isAllowedMarkdownDocPath(const std::filesystem::path& abs,
                              const std::filesystem::path& repo_root)
{
    if(repo_root.empty() || abs.empty())
        return false;
    std::error_code ec;
    const std::filesystem::path canon_root = std::filesystem::weakly_canonical(repo_root, ec);
    if(ec)
        return false;
    const std::filesystem::path canon_abs = std::filesystem::weakly_canonical(abs, ec);
    if(ec)
        return false;
    const std::string rel = repoRelativePosixPath(canon_abs, canon_root);
    if(rel.empty())
        return false;
    if(startsWithPosix(rel, "Docs") || startsWithPosix(rel, "Rdk/Docs")
       || startsWithPosix(rel, "Rdk/LLM/Docs") || startsWithPosix(rel, "Bin/Docs"))
        return true;
    for(const std::string& lib : allowedLibraryDocsRelRoots())
    {
        if(startsWithPosix(rel, lib))
            return true;
    }
    return false;
}

std::string repoRelativePosixPath(const std::filesystem::path& abs_or_rel,
                                  const std::filesystem::path& repo_root)
{
    if(repo_root.empty())
        return {};
    std::error_code ec;
    std::filesystem::path abs = abs_or_rel;
    if(!abs.is_absolute())
        abs = repo_root / abs_or_rel;
    abs = std::filesystem::weakly_canonical(abs, ec);
    if(ec)
        abs = (repo_root / abs_or_rel).lexically_normal();
    const std::filesystem::path root = std::filesystem::weakly_canonical(repo_root, ec);
    const std::filesystem::path use_root = ec ? repo_root.lexically_normal() : root;
    const std::filesystem::path rel = abs.lexically_relative(use_root);
    const std::string s = toPosix(rel);
    if(s.empty() || s == "." || s.rfind("..", 0) == 0)
        return {};
    return s;
}

std::string makeDocUriFromRepoRelative(const std::string& rel_posix)
{
    std::string rel = toPosix(rel_posix);
    if(rel.empty())
        return {};
    return "nmsdk-doc:" + rel;
}

std::string makeHelpUri(const std::string& topic)
{
    const std::string t = topic.empty() ? "index.html" : topic;
    return "nmsdk-help:" + t;
}

std::string makeClassUri(const std::string& class_name)
{
    return "nmsdk-class:" + class_name;
}

DocOpenResolve resolveMarkdownPath(const std::string& path_or_rel,
                                   const std::filesystem::path& repo_root)
{
    DocOpenResolve out;
    out.kind = "markdown";
    if(path_or_rel.empty() || repo_root.empty())
    {
        out.error_code = "INVALID_URI";
        return out;
    }
    std::filesystem::path abs = path_or_rel;
    if(!abs.is_absolute())
        abs = repo_root / path_or_rel;
    if(!isAllowedMarkdownDocPath(abs, repo_root))
    {
        out.error_code = "PATH_NOT_ALLOWED";
        return out;
    }
    std::error_code ec;
    const std::filesystem::path canon = std::filesystem::weakly_canonical(abs, ec);
    if(ec || !std::filesystem::is_regular_file(canon))
    {
        out.error_code = "NOT_FOUND";
        return out;
    }
    const std::string ext = toLowerAscii(canon.extension().string());
    if(ext != ".md" && ext != ".markdown" && ext != ".txt")
    {
        out.error_code = "PATH_NOT_ALLOWED";
        return out;
    }
    out.ok = true;
    out.abs_path = canon;
    out.doc_uri = makeDocUriFromRepoRelative(repoRelativePosixPath(canon, repo_root));
    return out;
}

DocOpenResolve resolveHelpTopic(const std::string& topic, const std::filesystem::path& repo_root,
                                const std::string& locale)
{
    DocOpenResolve out;
    out.kind = "help";
    if(repo_root.empty())
    {
        out.error_code = "INVALID_URI";
        return out;
    }
    if(!isSafeHelpTopic(topic))
    {
        out.error_code = "PATH_NOT_ALLOWED";
        return out;
    }
    std::string lang = locale.empty() ? "en" : locale;
    if(lang != "ru" && lang != "en")
        lang = "en";
    const std::string file = topic.empty() ? "index.html" : topic;
    const std::filesystem::path abs = repo_root / "Bin" / "Help" / lang / file;
    if(!std::filesystem::is_regular_file(abs))
    {
        // Prefer other locale if requested missing.
        const std::string alt = (lang == "ru") ? "en" : "ru";
        const std::filesystem::path alt_abs = repo_root / "Bin" / "Help" / alt / file;
        if(!std::filesystem::is_regular_file(alt_abs))
        {
            out.error_code = "NOT_FOUND";
            out.help_topic = file;
            return out;
        }
        out.abs_path = alt_abs;
    }
    else
        out.abs_path = abs;
    out.ok = true;
    out.help_topic = file;
    out.doc_uri = makeHelpUri(file);
    return out;
}

DocOpenResolve resolveDocUri(const std::string& uri, const std::filesystem::path& repo_root,
                             const std::string& help_locale)
{
    DocOpenResolve out;
    if(uri.empty())
    {
        out.error_code = "INVALID_URI";
        return out;
    }
    const auto colon = uri.find(':');
    if(colon == std::string::npos || colon == 0)
    {
        out.error_code = "INVALID_URI";
        return out;
    }
    const std::string scheme = toLowerAscii(uri.substr(0, colon));
    std::string rest = uri.substr(colon + 1);
    // Allow nmsdk-doc://path and nmsdk-doc:path
    if(rest.rfind("//", 0) == 0)
        rest = rest.substr(2);
    while(!rest.empty() && rest[0] == '/')
        rest.erase(rest.begin());

    if(scheme == "nmsdk-doc")
        return resolveMarkdownPath(rest, repo_root);
    if(scheme == "nmsdk-help")
        return resolveHelpTopic(rest, repo_root, help_locale);
    if(scheme == "nmsdk-class")
    {
        out.kind = "class";
        if(rest.empty() || rest.find('/') != std::string::npos || rest.find("..") != std::string::npos)
        {
            out.error_code = "INVALID_URI";
            return out;
        }
        out.ok = true;
        out.class_name = rest;
        out.doc_uri = makeClassUri(rest);
        return out;
    }
    out.error_code = "INVALID_URI";
    return out;
}

} // namespace RDK::LLM
