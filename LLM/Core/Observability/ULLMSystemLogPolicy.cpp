#include "ULLMSystemLogPolicy.h"

#include "../../../Core/Application/UApplication.h"
#include "../../../Core/Application/UProject.h"
#include "../../../Deploy/Include/rdk_init.h"

#include <sstream>

namespace RDK::LLM {

namespace {

ChannelLogPolicy channelPolicyFromFlags(int channel_index, bool events_log_mode, bool debug_mode)
{
    ChannelLogPolicy ch;
    ch.channel_index = channel_index;
    ch.debug_mode = debug_mode;
    ch.events_log_mode = events_log_mode;
    ch.effective_min_severity = determineBaseLogSeverity(events_log_mode, debug_mode);
    ch.effective_min_severity_name = severityName(ch.effective_min_severity);
    ch.max_verbosity = debug_mode ? 1 : 0;
    return ch;
}

std::string buildSummary(const SystemLogPolicySnapshot& snap)
{
    std::ostringstream oss;
    oss << "Текущая политика логов проекта: минимальный уровень "
        << snap.project_effective_min_severity_name << ". ";
    oss << "DebugMode проекта: " << (snap.project_debug_mode ? "вкл" : "выкл") << ". ";
    oss << "EventsLogMode: " << (snap.project_events_log_mode ? "вкл" : "выкл") << ". ";
    if(!snap.channels.empty())
    {
        const ChannelLogPolicy* active = nullptr;
        for(const ChannelLogPolicy& ch : snap.channels)
        {
            if(ch.channel_index == snap.active_channel_index)
            {
                active = &ch;
                break;
            }
        }
        if(active)
        {
            oss << "Активный канал " << snap.active_channel_index << ": минимальный уровень "
                << active->effective_min_severity_name << ". ";
        }
    }
    if(!snap.project_debug_mode)
    {
        oss << "Если DEBUG выключен, отладочные сообщения не записываются в файлы и не "
               "отображаются в окне лога — не ожидайте их в system_log_excerpt и read_system_log.";
    }
    return oss.str();
}

} // namespace

int determineBaseLogSeverity(bool events_log_mode, bool debug_mode)
{
    if(debug_mode)
        return RDK_EX_DEBUG;
    if(events_log_mode)
        return RDK_EX_INFO;
    return RDK_EX_WARNING;
}

std::string severityName(int rdk_severity)
{
    if(rdk_severity <= RDK_EX_FATAL)
        return "FATAL";
    if(rdk_severity == RDK_EX_ERROR)
        return "ERROR";
    if(rdk_severity == RDK_EX_WARNING)
        return "WARNING";
    if(rdk_severity == RDK_EX_INFO || rdk_severity == RDK_EX_APP)
        return "INFO";
    return "DEBUG";
}

SystemLogPolicySnapshot buildSystemLogPolicySnapshotFromConfig(const TProjectConfig& config,
                                                               int active_channel_index,
                                                               const nlohmann::json& log_paths_json)
{
    SystemLogPolicySnapshot snap;
    snap.project_debug_mode = config.DebugMode;
    snap.project_events_log_mode = config.EventsLogMode;
    snap.active_channel_index = active_channel_index;
    snap.project_effective_min_severity =
        determineBaseLogSeverity(config.EventsLogMode, config.DebugMode);
    snap.project_effective_min_severity_name = severityName(snap.project_effective_min_severity);

    const int num_channels = static_cast<int>(config.ChannelsConfig.size());
    for(int i = 0; i < num_channels; ++i)
    {
        const auto& channel_cfg = config.ChannelsConfig[i];
        const bool channel_debug = channel_cfg.DebugMode || config.DebugMode;
        const bool channel_info = channel_cfg.EventsLogMode || config.EventsLogMode;
        snap.channels.push_back(channelPolicyFromFlags(i, channel_info, channel_debug));
    }

    snap.details = {{"project",
                     {{"debug_mode", snap.project_debug_mode},
                      {"events_log_mode", snap.project_events_log_mode},
                      {"min_severity", snap.project_effective_min_severity_name}}},
                    {"active_channel_index", snap.active_channel_index},
                    {"channels", nlohmann::json::array()}};

    for(const ChannelLogPolicy& ch : snap.channels)
    {
        snap.details["channels"].push_back({{"index", ch.channel_index},
                                            {"debug_mode", ch.debug_mode},
                                            {"events_log_mode", ch.events_log_mode},
                                            {"min_severity", ch.effective_min_severity_name}});
    }
    if(!log_paths_json.is_null())
        snap.details["log_paths"] = log_paths_json;

    snap.summary_for_model = buildSummary(snap);
    return snap;
}

SystemLogPolicySnapshot buildSystemLogPolicySnapshot(const UApplication* app, int active_channel_index)
{
    if(!app)
        return buildSystemLogPolicySnapshotFromConfig(TProjectConfig{}, active_channel_index);

    nlohmann::json log_paths;
    const ApplicationLogReadPaths paths = app->GetApplicationLogReadPaths();
    log_paths["base_name"] = paths.base_name;
    log_paths["session_start_unix"] = paths.session_start_unix;
    log_paths["directories"] = nlohmann::json::array();
    for(const std::string& dir : paths.directories)
        log_paths["directories"].push_back(dir);

    return buildSystemLogPolicySnapshotFromConfig(app->GetProjectConfig(), active_channel_index,
                                                  log_paths);
}

} // namespace RDK::LLM
