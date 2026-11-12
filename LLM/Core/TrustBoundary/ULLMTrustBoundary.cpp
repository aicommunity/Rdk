#include "TrustBoundary/ULLMTrustBoundary.h"

#include <algorithm>
#include <cctype>
#include <vector>

namespace RDK::LLM {

std::string sanitizeUntrustedToolContent(const std::string& raw)
{
    std::string sanitized = raw;
    const std::vector<std::string> blocked_markers = {"ignore previous instructions",
                                                       "system prompt",
                                                       "developer message",
                                                       "<system>",
                                                       "<developer>",
                                                       "tool override"};
    for(const std::string& marker : blocked_markers)
    {
        std::string lower = sanitized;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        const std::string marker_lower = [&]() {
            std::string tmp = marker;
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return tmp;
        }();
        const size_t pos = lower.find(marker_lower);
        if(pos != std::string::npos)
            sanitized.replace(pos, marker.size(), "[filtered]");
    }
    constexpr size_t kMaxToolPayloadChars = 8000;
    if(sanitized.size() > kMaxToolPayloadChars)
        sanitized = sanitized.substr(0, kMaxToolPayloadChars) + "...[truncated]";
    return sanitized;
}

} // namespace RDK::LLM
