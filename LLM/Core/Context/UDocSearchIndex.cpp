#include "UDocSearchIndex.h"

#include "UDocSearchHelper.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cctype>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace RDK::LLM {

namespace {

constexpr int kEmbedDim = 64;
constexpr double kSemanticWeight = 0.35;

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

void projectEmbedding(const std::vector<std::string>& tokens, std::vector<float>& out)
{
    out.assign(static_cast<size_t>(kEmbedDim), 0.f);
    for(const std::string& tok : tokens)
    {
        size_t h = std::hash<std::string>{}(tok);
        for(int i = 0; i < kEmbedDim; ++i)
        {
            h ^= h >> 13;
            h *= 0x9e3779b97f4a7c15ULL;
            out[static_cast<size_t>(i)] += ((h & 1) ? 1.f : -1.f);
        }
    }
    double norm = 0.0;
    for(float v : out)
        norm += static_cast<double>(v) * static_cast<double>(v);
    norm = std::sqrt(norm);
    if(norm > 1e-9)
    {
        for(float& v : out)
            v = static_cast<float>(static_cast<double>(v) / norm);
    }
}

double cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b)
{
    if(a.size() != b.size() || a.empty())
        return 0.0;
    double dot = 0.0;
    for(size_t i = 0; i < a.size(); ++i)
        dot += static_cast<double>(a[i]) * static_cast<double>(b[i]);
    return dot;
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
            const std::vector<std::string> tokens = tokenize(excerpt + " " + rec.title);
            rec.term_freq.clear();
            for(const std::string& tok : tokens)
            {
                rec.term_freq[tok]++;
                if(rec.term_freq[tok] == 1)
                    m_doc_freq[tok]++;
            }
            rec.length = static_cast<int>(tokens.size());
            projectEmbedding(tokens, rec.embedding);
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

    std::vector<float> query_embed;
    projectEmbedding(qtokens, query_embed);

    std::vector<DocSnippet> ranked;
    double max_tfidf = 0.0;
    for(const DocRecord& doc : m_docs)
    {
        double tfidf = 0.0;
        for(const std::string& term : qtokens)
        {
            const auto tf_it = doc.term_freq.find(term);
            const auto df_it = m_doc_freq.find(term);
            if(tf_it == doc.term_freq.end() || df_it == m_doc_freq.end())
                continue;
            const double tf = static_cast<double>(tf_it->second);
            const double idf =
                std::log(1.0 + static_cast<double>(m_doc_count) / (1.0 + df_it->second));
            tfidf += tf * idf;
        }
        const double semantic = cosineSimilarity(query_embed, doc.embedding);
        const double hybrid = (1.0 - kSemanticWeight) * tfidf + kSemanticWeight * semantic;
        if(hybrid <= 0.0)
            continue;
        max_tfidf = std::max(max_tfidf, hybrid);
        DocSnippet sn;
        sn.path = doc.path;
        sn.title = doc.title;
        sn.excerpt = doc.excerpt;
        sn.score = hybrid;
        ranked.push_back(std::move(sn));
    }

    if(max_tfidf > 0.0)
    {
        for(DocSnippet& sn : ranked)
            sn.score /= max_tfidf;
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
