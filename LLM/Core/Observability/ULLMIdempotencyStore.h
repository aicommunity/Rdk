#ifndef RDK_ULLM_IDEMPOTENCY_STORE_H
#define RDK_ULLM_IDEMPOTENCY_STORE_H

#include <chrono>
#include <map>
#include <mutex>
#include <optional>
#include <string>

#include "../LlmTypes.h"

namespace RDK::LLM {

class ULLMIdempotencyStore {
public:
    std::optional<ToolGatewayResult> find(const std::string& key) const;
    void put(const std::string& key, const ToolGatewayResult& result,
             std::chrono::minutes ttl = std::chrono::minutes(15));

private:
    struct Entry {
        ToolGatewayResult result;
        std::chrono::steady_clock::time_point expires;
    };
    mutable std::mutex m_mutex;
    std::map<std::string, Entry> m_entries;
};

} // namespace RDK::LLM

#endif
