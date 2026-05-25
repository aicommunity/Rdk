#include "UDocSearchIndex.h"

#include "UDocSearchHelper.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

std::vector<std::string> tokenize(const std::string& text)
{
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string word;
    while(iss >> word)
    {
        std::string lower;
        lower.reserve(word.size());
        for(char c : word)
        {
            if(std::isalnum(static_cast<unsigned char>(c)))
                lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        if(lower.size() >= 2)
            tokens.push_back(lower);
    }
    return tokens;
}

} // namespace

void UDocSearchIndex::build(const std::vector<fs::path>& roots, int max_files)
{
    m_docs.clear();
    m_doc_freq.clear();
    m_doc_count = 0;

    for(const fs::path& root : roots)
    {
        if(!fs::exists(root))
            continue;
        std::error_code ec;
        for(fs::recursive_directory_iterator it(root, ec), end; it != end && m_doc_count < max_files;
            it.increment(ec))
        {
            if(ec)
                break;
            if(!it->is_regular_file())
                continue;
            const fs::path file = it->path();
            if(file.extension() != ".md" && file.extension() != ".txt")
                continue;

            std::ifstream in(file);
            std::string line;
            std::string excerpt;
            while(std::getline(in, line) && excerpt.size() < 800)
                excerpt += line + "\n";

            DocRecord rec;
            rec.path = file.string();
            rec.title = file.filename().string();
            rec.excerpt = excerpt.substr(0, 400);
            rec.term_freq.clear();
            for(const std::string& tok : tokenize(excerpt))
            {
                rec.term_freq[tok]++;
                if(rec.term_freq[tok] == 1)
                    m_doc_freq[tok]++;
            }
            rec.length = static_cast<int>(tokenize(excerpt).size());
            m_docs.push_back(std::move(rec));
            ++m_doc_count;
        }
    }
}

std::vector<DocSnippet> UDocSearchIndex::search(const std::string& query, int top_k) const
{
    const std::vector<std::string> qtokens = tokenize(query);
    if(qtokens.empty() || m_docs.empty())
        return {};

    std::vector<DocSnippet> ranked;
    for(const DocRecord& doc : m_docs)
    {
        double score = 0.0;
        for(const std::string& term : qtokens)
        {
            const auto tf_it = doc.term_freq.find(term);
            const auto df_it = m_doc_freq.find(term);
            if(tf_it == doc.term_freq.end() || df_it == m_doc_freq.end())
                continue;
            const double tf = static_cast<double>(tf_it->second);
            const double idf = std::log(1.0 + static_cast<double>(m_doc_count) / (1.0 + df_it->second));
            score += tf * idf;
        }
        if(score <= 0.0)
            continue;
        DocSnippet sn;
        sn.path = doc.path;
        sn.title = doc.title;
        sn.excerpt = doc.excerpt;
        sn.score = score;
        ranked.push_back(std::move(sn));
    }

    std::sort(ranked.begin(), ranked.end(),
              [](const DocSnippet& a, const DocSnippet& b) { return a.score > b.score; });
    if(static_cast<int>(ranked.size()) > top_k)
        ranked.resize(static_cast<size_t>(top_k));
    return ranked;
}

std::vector<DocSnippet> searchDocsWithIndex(const std::vector<fs::path>& roots, const std::string& query,
                                            int top_k)
{
    UDocSearchIndex index;
    index.build(roots, 600);
    std::vector<DocSnippet> hits = index.search(query, top_k);
    if(hits.empty())
        hits = UDocSearchHelper::searchRoots(roots, query, top_k);
    return hits;
}

} // namespace RDK::LLM
