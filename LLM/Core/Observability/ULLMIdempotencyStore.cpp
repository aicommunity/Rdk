#include "ULLMIdempotencyStore.h"

namespace RDK::LLM {

std::optional<ToolGatewayResult> ULLMIdempotencyStore::find(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_entries.find(key);
    if(it == m_entries.end())
        return std::nullopt;
    if(std::chrono::steady_clock::now() > it->second.expires)
        return std::nullopt;
    return it->second.result;
}

void ULLMIdempotencyStore::put(const std::string& key, const ToolGatewayResult& result,
                               std::chrono::minutes ttl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    Entry e;
    e.result = result;
    e.expires = std::chrono::steady_clock::now() + ttl;
    m_entries[key] = e;
}

} // namespace RDK::LLM
