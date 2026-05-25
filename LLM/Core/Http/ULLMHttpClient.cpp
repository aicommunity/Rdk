#include "ULLMHttpClient.h"

#include <curl/curl.h>

namespace RDK::LLM {

namespace {

struct WriteContext {
    std::string* body = nullptr;
    std::string line_buffer;
    ULLMHttpClient::SseDataCallback on_sse;
    std::atomic<bool>* cancelled = nullptr;
};

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto* ctx = static_cast<WriteContext*>(userdata);
    if(ctx->cancelled && ctx->cancelled->load())
        return 0;

    const size_t total = size * nmemb;
    if(ctx->body)
        ctx->body->append(ptr, total);

    if(ctx->on_sse)
    {
        ctx->line_buffer.append(ptr, total);
        for(;;)
        {
            const auto pos = ctx->line_buffer.find('\n');
            if(pos == std::string::npos)
                break;
            std::string line = ctx->line_buffer.substr(0, pos);
            ctx->line_buffer.erase(0, pos + 1);
            while(!line.empty() && (line.back() == '\r' || line.back() == ' '))
                line.pop_back();
            if(line.rfind("data:", 0) == 0)
            {
                std::string payload = line.substr(5);
                while(!payload.empty() && payload.front() == ' ')
                    payload.erase(payload.begin());
                if(payload == "[DONE]")
                    continue;
                if(!ctx->on_sse(payload))
                    return 0;
            }
        }
    }
    return total;
}

} // namespace

ULLMHttpClient::Response ULLMHttpClient::get(const std::string& url, const std::string& bearer_token,
                                            int timeout_ms)
{
    resetCancel();
    Response response;
    WriteContext ctx;
    ctx.body = &response.body;
    ctx.cancelled = &m_cancelled;

    CURL* curl = curl_easy_init();
    if(!curl)
    {
        response.error = "curl_easy_init failed";
        return response;
    }

    struct curl_slist* headers = nullptr;
    if(!bearer_token.empty())
    {
        const std::string auth = "Authorization: Bearer " + bearer_token;
        headers = curl_slist_append(headers, auth.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    if(headers)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_ms));

    const CURLcode code = curl_easy_perform(curl);
    if(code != CURLE_OK)
        response.error = curl_easy_strerror(code);
    else
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

ULLMHttpClient::Response ULLMHttpClient::postJson(const std::string& url,
                                                 const std::string& json_body,
                                                 const std::string& bearer_token,
                                                 int timeout_ms)
{
    resetCancel();
    Response response;
    WriteContext ctx;
    ctx.body = &response.body;
    ctx.cancelled = &m_cancelled;

    CURL* curl = curl_easy_init();
    if(!curl)
    {
        response.error = "curl_easy_init failed";
        return response;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if(!bearer_token.empty())
    {
        const std::string auth = "Authorization: Bearer " + bearer_token;
        headers = curl_slist_append(headers, auth.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_ms));

    const CURLcode code = curl_easy_perform(curl);
    if(code != CURLE_OK)
        response.error = curl_easy_strerror(code);
    else
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

ULLMHttpClient::Response ULLMHttpClient::postJsonStream(const std::string& url,
                                                       const std::string& json_body,
                                                       const std::string& bearer_token,
                                                       SseDataCallback on_data,
                                                       int timeout_ms)
{
    resetCancel();
    Response response;
    WriteContext ctx;
    ctx.body = &response.body;
    ctx.on_sse = std::move(on_data);
    ctx.cancelled = &m_cancelled;

    CURL* curl = curl_easy_init();
    if(!curl)
    {
        response.error = "curl_easy_init failed";
        return response;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: text/event-stream");
    if(!bearer_token.empty())
    {
        const std::string auth = "Authorization: Bearer " + bearer_token;
        headers = curl_slist_append(headers, auth.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_ms));

    const CURLcode code = curl_easy_perform(curl);
    if(code != CURLE_OK)
        response.error = curl_easy_strerror(code);
    else
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

} // namespace RDK::LLM
