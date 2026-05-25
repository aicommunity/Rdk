#include "UDocSearchHelper.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <system_error>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<std::string> tokenize(const std::string& query)
{
    std::vector<std::string> tokens;
    std::istringstream iss(toLower(query));
    std::string word;
    while(iss >> word)
    {
        if(word.size() >= 2)
            tokens.push_back(word);
    }
    return tokens;
}

double scoreText(const std::string& haystack_lower, const std::string& filename_lower,
                 const std::vector<std::string>& tokens)
{
    if(tokens.empty())
        return 0.0;
    double score = 0.0;
    for(const std::string& t : tokens)
    {
        if(haystack_lower.find(t) != std::string::npos)
            score += 1.0;
        if(filename_lower.find(t) != std::string::npos)
            score += 0.5;
    }
    return score / static_cast<double>(tokens.size());
}

} // namespace

std::vector<DocSnippet> UDocSearchHelper::searchRoots(const std::vector<fs::path>& roots,
                                                      const std::string& query, int top_k)
{
    std::vector<DocSnippet> ranked;
    const std::vector<std::string> tokens = tokenize(query);
    if(tokens.empty())
        return ranked;

    auto consider = [&](const fs::path& file) {
        if(!file.has_extension())
            return;
        const std::string ext = toLower(file.extension().string());
        if(ext != ".md" && ext != ".txt")
            return;
        std::ifstream in(file);
        if(!in)
            return;
        std::string line;
        std::string excerpt;
        while(std::getline(in, line) && excerpt.size() < 600)
            excerpt += line + "\n";
        const std::string lower_excerpt = toLower(excerpt);
        const std::string lower_name = toLower(file.filename().string());
        const double score = scoreText(lower_excerpt, lower_name, tokens);
        if(score <= 0.0)
            return;
        DocSnippet sn;
        sn.path = file.string();
        sn.title = file.filename().string();
        sn.excerpt = excerpt.substr(0, 400);
        sn.score = score;
        ranked.push_back(std::move(sn));
    };

    for(const fs::path& root : roots)
    {
        if(!fs::exists(root))
            continue;
        std::error_code ec;
        for(fs::recursive_directory_iterator it(root, ec), end; it != end; it.increment(ec))
        {
            if(ec)
                break;
            if(it->is_regular_file())
                consider(it->path());
        }
    }

    std::sort(ranked.begin(), ranked.end(),
              [](const DocSnippet& a, const DocSnippet& b) { return a.score > b.score; });
    if(static_cast<int>(ranked.size()) > top_k)
        ranked.resize(static_cast<size_t>(top_k));
    return ranked;
}

} // namespace RDK::LLM
