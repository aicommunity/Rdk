#ifndef RDK_ULLM_HTTP_CLIENT_H
#define RDK_ULLM_HTTP_CLIENT_H

#include <atomic>
#include <functional>
#include <string>

namespace RDK::LLM {

class ULLMHttpClient {
public:
    struct Response {
        long status_code = 0;
        std::string body;
        std::string error;
    };

    /// Invoked for each SSE `data:` payload (without prefix). Return false to abort.
    using SseDataCallback = std::function<bool(const std::string& data_payload)>;

    Response postJson(const std::string& url, const std::string& json_body,
                      const std::string& bearer_token, int timeout_ms = 120000);
    Response postJsonStream(const std::string& url, const std::string& json_body,
                            const std::string& bearer_token, SseDataCallback on_data,
                            int timeout_ms = 120000);
    void cancel() { m_cancelled = true; }
    void resetCancel() { m_cancelled = false; }

private:
    std::atomic<bool> m_cancelled{false};
};

} // namespace RDK::LLM

#endif
