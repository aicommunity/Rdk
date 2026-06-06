#include "UDocOllamaEmbeddings.h"

#include "../Http/ULLMHttpClient.h"

#include <cmath>
#include <cstdlib>
#include <nlohmann/json.hpp>

namespace RDK::LLM {

namespace {

std::string ollamaBaseUrl()
{
    if(const char* env = std::getenv("NMSDK_LLM_OLLAMA_URL"))
        return env;
    return "http://127.0.0.1:11434";
}

std::string embedModel()
{
    if(const char* env = std::getenv("NMSDK_LLM_OLLAMA_EMBED_MODEL"))
        return env;
    return "nomic-embed-text";
}

double cosine(const std::vector<float>& a, const std::vector<float>& b)
{
    if(a.size() != b.size() || a.empty())
        return 0.0;
    double dot = 0.0;
    for(size_t i = 0; i < a.size(); ++i)
        dot += static_cast<double>(a[i]) * static_cast<double>(b[i]);
    return dot;
}

} // namespace

bool UDocOllamaEmbeddings::enabled()
{
    const char* env = std::getenv("NMSDK_LLM_DOC_EMBED_OLLAMA");
    return env && env[0] == '1';
}

bool UDocOllamaEmbeddings::embedText(const std::string& text, std::vector<float>& out)
{
    out.clear();
    if(text.empty())
        return false;

    std::string url = ollamaBaseUrl();
    if(url.back() == '/')
        url.pop_back();
    url += "/api/embeddings";

    nlohmann::json body;
    body["model"] = embedModel();
    body["prompt"] = text.substr(0, 2048);

    ULLMHttpClient http;
    const auto resp = http.postJson(url, body.dump(), "", 30000);
    if(!resp.error.empty() || resp.status_code < 200 || resp.status_code >= 300)
        return false;

    try
    {
        const nlohmann::json j = nlohmann::json::parse(resp.body);
        if(!j.contains("embedding") || !j["embedding"].is_array())
            return false;
        for(const auto& v : j["embedding"])
            out.push_back(v.get<float>());
        return !out.empty();
    }
    catch(...)
    {
        return false;
    }
}

void UDocOllamaEmbeddings::rerank(const std::string& query, std::vector<DocSnippet>& hits, int top_k)
{
    if(!enabled() || hits.empty() || top_k <= 0)
        return;

    std::vector<float> query_vec;
    if(!embedText(query, query_vec))
        return;

    for(DocSnippet& hit : hits)
    {
        const std::string doc_text = hit.title + "\n" + hit.excerpt;
        std::vector<float> doc_vec;
        const double semantic = embedText(doc_text, doc_vec) ? cosine(query_vec, doc_vec) : 0.0;
        hit.score = 0.55 * hit.score + 0.45 * semantic;
    }

    std::sort(hits.begin(), hits.end(),
              [](const DocSnippet& a, const DocSnippet& b) { return a.score > b.score; });
    if(static_cast<int>(hits.size()) > top_k)
        hits.resize(static_cast<size_t>(top_k));
}

} // namespace RDK::LLM
