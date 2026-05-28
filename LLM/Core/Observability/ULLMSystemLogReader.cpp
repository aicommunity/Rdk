#include "ULLMSystemLogReader.h"

#include "../TrustBoundary/ULLMTrustBoundary.h"
#include "../../../Core/Application/UApplication.h"
#include "../../../Deploy/Include/rdk_init.h"

#include <cstdlib>
#include <sstream>

namespace RDK::LLM {

namespace {

std::size_t parseEnvSize(const char* name, std::size_t default_value)
{
    if(const char* value = std::getenv(name))
    {
        try
        {
            const int parsed = std::stoi(value);
            if(parsed > 0)
                return static_cast<std::size_t>(parsed);
        }
        catch(...)
        {
        }
    }
    return default_value;
}

char severityLetter(int severity)
{
    if(severity <= RDK_EX_FATAL)
        return 'F';
    if(severity == RDK_EX_ERROR)
        return 'E';
    if(severity == RDK_EX_WARNING)
        return 'W';
    if(severity == RDK_EX_INFO || severity == RDK_EX_APP)
        return 'I';
    return 'D';
}

} // namespace

ULLMSystemLogReader::ULLMSystemLogReader(RDK::UApplication* app)
    : m_app(app)
{
}

bool ULLMSystemLogReader::isFeatureEnabled()
{
    if(const char* disable = std::getenv("NMSDK_LLM_DISABLE_SYSTEM_LOG"))
        return !(disable[0] == '1' && disable[1] == '\0');
    return true;
}

std::size_t ULLMSystemLogReader::autoInjectMaxLines()
{
    return parseEnvSize("NMSDK_LLM_SYSTEM_LOG_AUTO_MAX_LINES", 40);
}

std::size_t ULLMSystemLogReader::toolReadMaxLines()
{
    return parseEnvSize("NMSDK_LLM_SYSTEM_LOG_TOOL_MAX_LINES", 200);
}

int ULLMSystemLogReader::defaultMinSeverityForPolicy(const SystemLogPolicySnapshot& policy)
{
    return policy.project_effective_min_severity;
}

int ULLMSystemLogReader::parseSeverityToken(const std::string& token, int fallback)
{
    if(token == "DEBUG")
        return RDK_EX_DEBUG;
    if(token == "INFO")
        return RDK_EX_INFO;
    if(token == "WARNING")
        return RDK_EX_WARNING;
    if(token == "ERROR")
        return RDK_EX_ERROR;
    if(token == "FATAL")
        return RDK_EX_FATAL;
    return fallback;
}

void ULLMSystemLogReader::syncPaths()
{
    if(!m_app)
        return;
    m_tail.syncFromPaths(m_app->GetApplicationLogReadPaths());
}

SystemLogPolicySnapshot ULLMSystemLogReader::policy(int active_channel) const
{
    if(!m_app)
        return SystemLogPolicySnapshot{};
    if(m_cached_policy_channel == active_channel)
        return m_cached_policy;
    m_cached_policy_channel = active_channel;
    m_cached_policy = buildSystemLogPolicySnapshot(m_app, active_channel);
    return m_cached_policy;
}

void ULLMSystemLogReader::mark()
{
    m_tail.mark();
}

std::string ULLMSystemLogReader::formatLines(const std::vector<RDK::LogTailLine>& lines) const
{
    std::ostringstream oss;
    constexpr std::size_t kMaxLineChars = 512;
    constexpr std::size_t kMaxTotalChars = 24 * 1024;
    std::size_t total = 0;

    for(const RDK::LogTailLine& line : lines)
    {
        std::string text = line.text;
        if(text.size() > kMaxLineChars)
            text.resize(kMaxLineChars);

        std::ostringstream row;
        row << '[' << severityLetter(line.severity) << "] " << text;
        std::string formatted = sanitizeUntrustedToolContent(row.str());
        if(total + formatted.size() + 1 > kMaxTotalChars)
            break;
        if(!oss.str().empty())
            oss << '\n';
        oss << formatted;
        total += formatted.size() + 1;
    }
    return oss.str();
}

std::string ULLMSystemLogReader::collectDeltaExcerpt(std::size_t max_lines, int min_severity) const
{
    return formatLines(m_tail.collectDelta(max_lines, min_severity));
}

std::string ULLMSystemLogReader::readTailExcerpt(std::size_t max_lines, int min_severity) const
{
    return formatLines(m_tail.readTail(max_lines, min_severity));
}

} // namespace RDK::LLM
