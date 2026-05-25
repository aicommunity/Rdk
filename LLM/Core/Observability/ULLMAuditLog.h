#ifndef RDK_ULLM_AUDIT_LOG_H
#define RDK_ULLM_AUDIT_LOG_H

#include <fstream>
#include <mutex>
#include <string>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class ULLMAuditLog {
public:
    void append(const std::string& event_type, const nlohmann::json& details,
                const std::string& trace_id, const std::string& session_id);
    void setLogDirectory(const std::string& path);

private:
    std::string m_prev_hash = "genesis";
    std::mutex m_mutex;
    std::string m_log_dir;
    std::ofstream m_stream;
};

} // namespace RDK::LLM

#endif
