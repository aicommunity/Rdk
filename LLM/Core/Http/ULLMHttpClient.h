#ifndef RDK_ULLM_HTTP_CLIENT_H
#define RDK_ULLM_HTTP_CLIENT_H

#include <atomic>
#include <string>

namespace RDK::LLM {

class ULLMHttpClient {
public:
    struct Response {
        long status_code = 0;
        std::string body;
        std::string error;
    };

    Response postJson(const std::string& url, const std::string& json_body,
                      const std::string& bearer_token, int timeout_ms = 120000);
    void cancel() { m_cancelled = true; }

private:
    std::atomic<bool> m_cancelled{false};
};

} // namespace RDK::LLM

#endif
