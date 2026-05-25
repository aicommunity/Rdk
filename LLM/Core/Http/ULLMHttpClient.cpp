#include "ULLMHttpClient.h"

#include <curl/curl.h>

namespace RDK::LLM {

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

ULLMHttpClient::Response ULLMHttpClient::postJson(const std::string& url,
                                                 const std::string& json_body,
                                                 const std::string& bearer_token,
                                                 int timeout_ms)
{
    Response response;
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
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
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
