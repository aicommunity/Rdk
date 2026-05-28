#ifndef RDK_ULLM_SYSTEM_LOG_POLICY_H
#define RDK_ULLM_SYSTEM_LOG_POLICY_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace RDK {
class UApplication;
struct TProjectConfig;
}

namespace RDK::LLM {

struct ChannelLogPolicy {
    int channel_index = 0;
    bool debug_mode = false;
    bool events_log_mode = false;
    int effective_min_severity = 0;
    std::string effective_min_severity_name;
    int max_verbosity = 0;
};

struct SystemLogPolicySnapshot {
    bool project_debug_mode = false;
    bool project_events_log_mode = false;
    int active_channel_index = 0;
    int project_effective_min_severity = 0;
    std::string project_effective_min_severity_name;
    std::vector<ChannelLogPolicy> channels;
    std::string summary_for_model;
    nlohmann::json details;
};

int determineBaseLogSeverity(bool events_log_mode, bool debug_mode);
std::string severityName(int rdk_severity);
SystemLogPolicySnapshot buildSystemLogPolicySnapshotFromConfig(const TProjectConfig& config,
                                                               int active_channel_index,
                                                               const nlohmann::json& log_paths_json =
                                                                   nlohmann::json::object());
SystemLogPolicySnapshot buildSystemLogPolicySnapshot(const UApplication* app,
                                                     int active_channel_index);

} // namespace RDK::LLM

#endif
