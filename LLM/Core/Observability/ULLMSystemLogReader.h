#ifndef RDK_ULLM_SYSTEM_LOG_READER_H
#define RDK_ULLM_SYSTEM_LOG_READER_H

#include <cstddef>
#include <string>

#include "../../../Core/Engine/UReadOnlyLogTail.h"
#include "ULLMSystemLogPolicy.h"

namespace RDK {
class UApplication;
}

namespace RDK::LLM {

class ULLMSystemLogReader {
public:
    explicit ULLMSystemLogReader(RDK::UApplication* app);

    void syncPaths();
    SystemLogPolicySnapshot policy(int active_channel) const;
    void mark();
    std::string collectDeltaExcerpt(std::size_t max_lines, int min_severity) const;
    std::string readTailExcerpt(std::size_t max_lines, int min_severity) const;

    static bool isFeatureEnabled();
    static std::size_t autoInjectMaxLines();
    static std::size_t toolReadMaxLines();
    static int defaultMinSeverityForPolicy(const SystemLogPolicySnapshot& policy);
    static int parseSeverityToken(const std::string& token, int fallback);

private:
    RDK::UApplication* m_app = nullptr;
    RDK::UReadOnlyLogTail m_tail;
    mutable int m_cached_policy_channel = -1;
    mutable SystemLogPolicySnapshot m_cached_policy;

    std::string formatLines(const std::vector<RDK::LogTailLine>& lines) const;
};

} // namespace RDK::LLM

#endif
