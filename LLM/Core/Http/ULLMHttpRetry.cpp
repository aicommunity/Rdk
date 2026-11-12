#include "Http/ULLMHttpRetry.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

int parseRetryAfterDelayMs(const std::string& retry_after_header)
{
    if(retry_after_header.empty())
        return 0;
    size_t start = 0;
    while(start < retry_after_header.size()
          && std::isspace(static_cast<unsigned char>(retry_after_header[start])))
        ++start;
    size_t end = start;
    while(end < retry_after_header.size()
          && std::isdigit(static_cast<unsigned char>(retry_after_header[end])))
        ++end;
    if(end == start)
        return 0;
    try
    {
        const int seconds = std::stoi(retry_after_header.substr(start, end - start));
        if(seconds <= 0)
            return 0;
        return seconds * 1000;
    }
    catch(...)
    {
        return 0;
    }
}

int providerRetryBackoffMs(int attempt)
{
    const int base = 300;
    const int jitter = 75 * (attempt + 1);
    return base * (1 << attempt) + jitter;
}

int providerRetryDelayMs(int attempt, const std::string& retry_after_header, int max_delay_ms)
{
    const int backoff = providerRetryBackoffMs(attempt);
    const int from_header = parseRetryAfterDelayMs(retry_after_header);
    return std::min(max_delay_ms, std::max(backoff, from_header));
}

} // namespace RDK::LLM
