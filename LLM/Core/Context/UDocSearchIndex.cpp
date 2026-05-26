#include "UDocSearchIndex.h"

#include "UDocOllamaEmbeddings.h"
#include "UDocSearchCatalog.h"
#include "UDocSearchHelper.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <nlohmann/json.hpp>

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
        for(unsigned char c : word)
        {
            if(std::isalnum(c))
                lower += static_cast<char>(std::tolower(c));
            else if(c >= 0x80)
                lower += static_cast<char>(c);
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

LLMContentKind parseContentKind(const std::string& name)
{
    if(name == "source")
        return LLMContentKind::Source;
    if(name == "runtime_xml")
        return LLMContentKind::RuntimeXml;
    return LLMContentKind::Doc;
}

std::string contentKindName(const LLMContentKind kind)
{
    switch(kind)
    {
    case LLMContentKind::Source:
        return "source";
    case LLMContentKind::RuntimeXml:
        return "runtime_xml";
    default:
        return "doc";
    }
}

} // namespace

void UDocSearchIndex::indexDocument(DocRecord rec)
{
    const std::vector<std::string> tokens = tokenize(rec.excerpt + " " + rec.title + " " + rec.path);
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
        for(fs::recursive_directory_iterator it(root, ec), end;
            it != end && m_doc_count < max_files; it.increment(ec))
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
            rec.source_id = "legacy";
            rec.path = file.string();
            rec.title = file.filename().string();
            rec.excerpt = excerpt.substr(0, 400);
            rec.content_kind = LLMContentKind::Doc;
            indexDocument(std::move(rec));
        }
    }
}

void UDocSearchIndex::buildFromCatalog(const ILLMKnowledgeCatalog& catalog,
                                       const fs::path& repository_root, int max_files)
{
    m_docs.clear();
    m_doc_freq.clear();
    m_doc_count = 0;

    for(const LLMKnowledgeSource& source : catalog.sources())
    {
        if(!fs::exists(source.root) || m_doc_count >= max_files)
            continue;

        if(!source.include_files_only.empty())
        {
            for(const std::string& rel : source.include_files_only)
            {
                if(m_doc_count >= max_files)
                    break;
                const fs::path file = source.root / rel;
                if(!fs::is_regular_file(file) || !extensionMatches(file, source.extensions))
                    continue;
                if(shouldExcludePath(repository_root, file, source.exclude_globs))
                    continue;

                std::ifstream in(file);
                std::stringstream buffer;
                buffer << in.rdbuf();
                const std::string content = buffer.str();

                DocRecord rec;
                rec.source_id = source.source_id;
                rec.path = makeRepoRelativePath(repository_root, file);
                rec.content_kind = contentKindForSource(source);
                rec.title = extractMarkdownTitle(content, file.filename().string());
                int start_line = 1;
                rec.excerpt = readSourceExcerpt(file, kSourceExcerptMaxLines, start_line);
                if(rec.excerpt.empty())
                    rec.excerpt = content.substr(0, 4000);
                rec.start_line = start_line;
                indexDocument(std::move(rec));
            }
            continue;
        }

        std::error_code ec;
        for(fs::recursive_directory_iterator it(source.root, ec), end;
            it != end && m_doc_count < max_files; it.increment(ec))
        {
            if(ec)
                break;
            if(!it->is_regular_file())
                continue;
            const fs::path file = it->path();
            if(!extensionMatches(file, source.extensions))
                continue;
            if(shouldExcludePath(repository_root, file, source.exclude_globs))
                continue;

            std::ifstream in(file);
            std::stringstream buffer;
            buffer << in.rdbuf();
            const std::string content = buffer.str();

            DocRecord rec;
            rec.source_id = source.source_id;
            rec.path = makeRepoRelativePath(repository_root, file);
            rec.content_kind = contentKindForSource(source);
            rec.title = extractMarkdownTitle(content, file.filename().string());
            if(rec.content_kind == LLMContentKind::Source)
            {
                int start_line = 1;
                rec.excerpt = readSourceExcerpt(file, kSourceExcerptMaxLines, start_line);
                rec.start_line = start_line;
            }
            else
                rec.excerpt = content.substr(0, 4000);
            indexDocument(std::move(rec));
        }
    }
}

bool UDocSearchIndex::loadPrebuilt(const fs::path& dir, const std::string& expected_fingerprint)
{
    const fs::path manifest_path = dir / "index-manifest.json";
    const fs::path data_path = dir / "index.jsonl";
    if(!fs::is_regular_file(manifest_path) || !fs::is_regular_file(data_path))
        return false;

    std::ifstream manifest_in(manifest_path);
    nlohmann::json manifest = nlohmann::json::parse(manifest_in, nullptr, false);
    if(manifest.is_discarded())
        return false;
    if(manifest.value("catalog_fingerprint", std::string()) != expected_fingerprint)
        return false;

    m_docs.clear();
    m_doc_freq.clear();
    m_doc_count = 0;

    std::ifstream data_in(data_path);
    std::string line;
    while(std::getline(data_in, line))
    {
        if(line.empty())
            continue;
        nlohmann::json row = nlohmann::json::parse(line, nullptr, false);
        if(row.is_discarded())
            continue;
        DocRecord rec;
        rec.source_id = row.value("source_id", "");
        rec.path = row.value("path", "");
        rec.title = row.value("title", "");
        rec.excerpt = row.value("excerpt", "");
        rec.content_kind = parseContentKind(row.value("content_kind", "doc"));
        rec.start_line = row.value("start_line", 0);
        indexDocument(std::move(rec));
    }
    return !m_docs.empty();
}

void UDocSearchIndex::savePrebuilt(const fs::path& dir, const std::string& fingerprint) const
{
    std::error_code ec;
    fs::create_directories(dir, ec);

    std::ofstream data_out(dir / "index.jsonl");
    for(const DocRecord& doc : m_docs)
    {
        nlohmann::json row = {{"source_id", doc.source_id},
                              {"path", doc.path},
                              {"title", doc.title},
                              {"excerpt", doc.excerpt},
                              {"content_kind", contentKindName(doc.content_kind)},
                              {"start_line", doc.start_line}};
        data_out << row.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace) << '\n';
    }

    nlohmann::json manifest = {{"catalog_fingerprint", fingerprint},
                               {"chunk_count", m_docs.size()}};
    std::ofstream manifest_out(dir / "index-manifest.json");
    manifest_out << manifest.dump(2);
}

std::vector<DocSnippet> UDocSearchIndex::searchInternal(const std::string& query, int top_k,
                                                        bool include_doc, bool include_source,
                                                        bool include_runtime) const
{
    const std::vector<std::string> qtokens = tokenize(query);
    if(qtokens.empty() || m_docs.empty())
        return {};

    std::vector<float> query_embed;
    projectEmbedding(qtokens, query_embed);

    std::vector<DocSnippet> ranked;
    double max_score = 0.0;
    for(const DocRecord& doc : m_docs)
    {
        if(doc.content_kind == LLMContentKind::Doc && !include_doc)
            continue;
        if(doc.content_kind == LLMContentKind::Source && !include_source)
            continue;
        if(doc.content_kind == LLMContentKind::RuntimeXml && !include_runtime)
            continue;

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
        max_score = std::max(max_score, hybrid);
        DocSnippet sn;
        sn.source_id = doc.source_id;
        sn.path = doc.path;
        sn.title = doc.title;
        sn.excerpt = doc.excerpt;
        sn.score = hybrid;
        sn.content_kind = doc.content_kind;
        sn.start_line = doc.start_line;
        ranked.push_back(std::move(sn));
    }

    if(max_score > 0.0)
    {
        for(DocSnippet& sn : ranked)
            sn.score /= max_score;
    }

    std::sort(ranked.begin(), ranked.end(),
              [](const DocSnippet& a, const DocSnippet& b) { return a.score > b.score; });
    if(static_cast<int>(ranked.size()) > top_k)
        ranked.resize(static_cast<size_t>(top_k));
    return ranked;
}

std::vector<DocSnippet> UDocSearchIndex::search(const std::string& query, int top_k,
                                                const LLMContentKind kind_filter) const
{
    switch(kind_filter)
    {
    case LLMContentKind::Source:
        return searchInternal(query, top_k, false, true, false);
    case LLMContentKind::RuntimeXml:
        return searchInternal(query, top_k, false, false, true);
    default:
        return searchInternal(query, top_k, true, false, false);
    }
}

std::vector<DocSnippet> UDocSearchIndex::searchWithScope(const std::string& query, int top_k,
                                                         const std::string& scope) const
{
    if(scope == "sources")
        return searchInternal(query, top_k, false, true, false);
    if(scope == "all")
        return searchInternal(query, top_k, true, true, false);
    return searchInternal(query, top_k, true, false, false);
}

std::vector<DocSnippet> searchDocsWithIndex(const std::vector<fs::path>& roots,
                                            const std::string& query, int top_k)
{
    UDocSearchIndex index;
    index.build(roots, 600);
    const int candidate_k = std::max(top_k * 4, 12);
    std::vector<DocSnippet> hits = index.search(query, candidate_k, LLMContentKind::Doc);
    if(hits.empty())
        hits = UDocSearchHelper::searchRoots(roots, query, candidate_k);
    if(!hits.empty())
    {
        if(UDocOllamaEmbeddings::enabled())
            UDocOllamaEmbeddings::rerank(query, hits, top_k);
        else if(static_cast<int>(hits.size()) > top_k)
            hits.resize(static_cast<size_t>(top_k));
    }
    hits.erase(std::remove_if(hits.begin(), hits.end(),
                              [](const DocSnippet& sn) { return sn.score < kMinRetrievalScore; }),
               hits.end());
    return hits;
}

} // namespace RDK::LLM
