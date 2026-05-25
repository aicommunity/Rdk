#include "ULLMAuditLog.h"
#include "../LlmTypes.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace RDK::LLM {

static std::string sha256_placeholder(const std::string& input)
{
    std::hash<std::string> h;
    std::ostringstream oss;
    oss << std::hex << h(input);
    return oss.str();
}

void ULLMAuditLog::setLogDirectory(const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_log_dir = path;
    std::filesystem::create_directories(m_log_dir);
}

void ULLMAuditLog::append(const std::string& event_type, const nlohmann::json& details,
                          const std::string& trace_id, const std::string& session_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    nlohmann::json event;
    event["event_type"] = event_type;
    event["trace_id"] = trace_id;
    event["session_id"] = session_id;
    event["tool_registry_version"] = TOOL_REGISTRY_VERSION;
    event["prompt_bundle_id"] = PROMPT_BUNDLE_ID;
    event["details"] = details;
    event["prev_hash"] = m_prev_hash;
    const std::string payload = event.dump();
    event["curr_hash"] = sha256_placeholder(m_prev_hash + payload);
    m_prev_hash = event["curr_hash"].get<std::string>();

    if(m_log_dir.empty())
        return;
    const std::string file = m_log_dir + "/audit.jsonl";
    std::ofstream out(file, std::ios::app);
    if(out)
        out << event.dump() << '\n';
}

} // namespace RDK::LLM
