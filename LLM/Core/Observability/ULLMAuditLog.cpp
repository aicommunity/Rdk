#include "ULLMAuditLog.h"

#include "ULLMAuditSanitizer.h"
#include "../LlmTypes.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <QByteArray>
#include <QCryptographicHash>

namespace RDK::LLM {

static std::string sha256_hex(const std::string& input)
{
    const QByteArray data(input.data(), static_cast<int>(input.size()));
    const QByteArray digest = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
    return digest.toStdString();
}

std::string ULLMAuditLog::computeEventHash(const std::string& prev_hash, const nlohmann::json& event)
{
    nlohmann::json payload = event;
    payload.erase("prev_hash");
    payload.erase("curr_hash");
    return sha256_hex(prev_hash
                      + payload.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace));
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
    event["details"] = sanitizeAuditDetails(details);
    event["prev_hash"] = m_prev_hash;
    event["curr_hash"] = computeEventHash(m_prev_hash, event);
    m_prev_hash = event["curr_hash"].get<std::string>();

    if(m_log_dir.empty())
        return;
    const std::string file = m_log_dir + "/audit.jsonl";
    std::ofstream out(file, std::ios::app);
    if(out)
        out << event.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace) << '\n';
}

AuditVerifyReport ULLMAuditLog::verifyJsonlFile(const std::string& path)
{
    AuditVerifyReport report;
    std::ifstream in(path);
    if(!in)
    {
        report.lines_failed = 1;
        report.first_error = "Cannot open file: " + path;
        return report;
    }

    std::string prev_hash = "genesis";
    std::string line;
    while(std::getline(in, line))
    {
        if(line.empty())
            continue;
        try
        {
            nlohmann::json event = nlohmann::json::parse(line);
            const std::string recorded_prev = event.value("prev_hash", "");
            const std::string recorded_curr = event.value("curr_hash", "");
            if(recorded_prev != prev_hash)
            {
                ++report.lines_failed;
                if(report.first_error.empty())
                    report.first_error = "prev_hash mismatch at line " + std::to_string(report.lines_verified + 1);
                continue;
            }
            const std::string expected = computeEventHash(prev_hash, event);
            if(recorded_curr != expected)
            {
                ++report.lines_failed;
                if(report.first_error.empty())
                    report.first_error = "curr_hash mismatch at line " + std::to_string(report.lines_verified + 1);
                continue;
            }
            prev_hash = recorded_curr;
            ++report.lines_verified;
        }
        catch(const std::exception& ex)
        {
            ++report.lines_failed;
            if(report.first_error.empty())
                report.first_error = std::string("JSON parse error: ") + ex.what();
        }
    }
    return report;
}

} // namespace RDK::LLM
