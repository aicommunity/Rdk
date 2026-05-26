#include "URdkApplicationCommands.h"

#include "URdkApplicationCommandsJson.h"
#include "../Policy/ULLMPathPolicy.h"
#include "../LlmPublicApi.h"

#include <ctime>
#include <filesystem>
#include <unordered_set>
#include <vector>

#include <rdk_application.h>

#include "../../../Core/Application/UProject.h"
#include "../../../Core/Engine/TProjectLoadDiagnostics.h"
#include "../../../Core/System/rdk_system.h"
#include "../../../Core/Utilities/USupport.h"

namespace RDK::LLM {

namespace {

std::string joinPath(const std::string& dir, const std::string& file)
{
    if(dir.empty())
        return file;
    std::string d = dir;
    if(d.back() != '/' && d.back() != '\\')
        d.push_back('/');
    return d + file;
}

} // namespace

URdkApplicationCommands::URdkApplicationCommands(RDK::UApplication* app)
    : m_app(app)
{
}

DomainSessionInfo URdkApplicationCommands::sessionInfo() const
{
    DomainSessionInfo info;
    if(!m_app)
        return info;
    info.project_loaded = m_app->GetProjectOpenFlag();
    info.channel_count = m_app->GetNumChannels();
    info.engine_ready = info.channel_count > 0 || info.project_loaded;
    return info;
}

ApplicationCommandResult URdkApplicationCommands::fail(DomainStatusCode code,
                                                       const std::string& message) const
{
    ApplicationCommandResult r;
    r.status = {code, message};
    return r;
}

std::string URdkApplicationCommands::resolveDefaultConfigsRoot(RDK::UApplication* app)
{
    if(app)
        return app->GetDefaultConfigsDirectory();
    return ".";
}

std::string URdkApplicationCommands::ensureProjectIniPath(const std::string& path)
{
    if(path.empty())
        return path;
    std::filesystem::path p(path);
    if(p.filename().empty() || p.filename() == "." || p.filename() == "..")
        return joinPath(path, "project.ini");
    const std::string fname = p.filename().generic_string();
    if(fname.find(".ini") != std::string::npos)
        return path;
    return joinPath(path, "project.ini");
}

std::string URdkApplicationCommands::resolveConfigurationPathFromArgs(const nlohmann::json& args,
                                                                    std::string& err)
{
    err.clear();
    if(args.contains("project_ini_path"))
        return ensureProjectIniPath(args.at("project_ini_path").get<std::string>());
    if(args.contains("configuration_path"))
        return ensureProjectIniPath(args.at("configuration_path").get<std::string>());
    if(args.contains("project_path"))
        return ensureProjectIniPath(args.at("project_path").get<std::string>());
    err = "configuration path is required";
    return {};
}

std::string URdkApplicationCommands::resolveProjectIniPath(const nlohmann::json& args,
                                                         RDK::UApplication* app,
                                                         std::string& err)
{
    err.clear();
    if(args.contains("project_ini_path") || args.contains("configuration_path")
       || args.contains("project_path"))
    {
        const std::string direct = resolveConfigurationPathFromArgs(args, err);
        if(!direct.empty())
            return direct;
        return {};
    }

    const bool autocreate = args.value("autocreate_subdirectory", true);
    std::string parent = args.value("parent_directory", std::string());

    if(app)
        return app->PrepareNewProjectIniPath(autocreate, parent, &err);

    if(parent.empty())
        parent = resolveDefaultConfigsRoot(app);
    if(parent.empty())
    {
        err = "parent_directory is required when project_ini_path is omitted";
        return {};
    }

    std::string norm_err;
    parent = ULLMPathPolicy::normalizePath(parent, norm_err);
    if(parent.empty())
    {
        err = norm_err.empty() ? "invalid parent_directory" : norm_err;
        return {};
    }

    if(autocreate)
    {
        const std::time_t now = std::time(nullptr);
        const std::string folder = RDK::get_text_time(now, '.', '_');
        parent = joinPath(parent, "Autocreate" + folder);
        if(RDK::CreateNewDirectory(parent.c_str()) != 0)
        {
            err = "failed to create configuration directory";
            return {};
        }
    }

    return joinPath(parent, "project.ini");
}

RDK::TProjectConfig URdkApplicationCommands::buildProjectConfigFromJson(const nlohmann::json& args,
                                                                        RDK::UApplication* app,
                                                                        std::string& err)
{
    err.clear();
    RDK::TProjectConfig cfg;

    cfg.DebugMode = true;
    cfg.ProjectMode = 0;
    cfg.ProjectName = "Autocreated configuration";
    if(app)
    {
        cfg.UserId = app->GetUserId();
        cfg.UserName = app->GetUserName();
    }
    const std::time_t now = std::time(nullptr);
    cfg.CreationTime = RDK::get_text_time(now, '.', '_');
    cfg.ProjectType = 0;
    cfg.EventsLogFlag = true;
    cfg.CalcSourceTimeMode = 0;
    cfg.MultiThreadingMode = 1;
    cfg.MTUpdateInterfaceInterval = 100;
    cfg.DescriptionFileName = "README.md";

    if(args.contains("project_name"))
        cfg.ProjectName = args.at("project_name").get<std::string>();
    if(args.contains("project_description"))
        cfg.ProjectDescription = args.at("project_description").get<std::string>();
    if(args.contains("description_file_name"))
        cfg.DescriptionFileName = args.at("description_file_name").get<std::string>();
    if(args.contains("project_type"))
        cfg.ProjectType = args.at("project_type").get<int>();
    if(args.contains("project_mode"))
        cfg.ProjectMode = args.at("project_mode").get<int>();
    if(args.contains("multi_threading_mode"))
        cfg.MultiThreadingMode = args.at("multi_threading_mode").get<int>();
    if(args.contains("calc_source_time_mode"))
        cfg.CalcSourceTimeMode = args.at("calc_source_time_mode").get<int>();
    if(args.contains("events_log_flag"))
        cfg.EventsLogFlag = args.at("events_log_flag").get<bool>();
    if(args.contains("debug_mode"))
        cfg.DebugMode = args.at("debug_mode").get<bool>();

    int num_channels = 1;
    if(args.contains("num_channels"))
        num_channels = args.at("num_channels").get<int>();
    else if(args.contains("channels") && args.at("channels").is_array())
        num_channels = static_cast<int>(args.at("channels").size());
    if(num_channels < 1)
        num_channels = 1;
    cfg.SetNumChannels(num_channels);

    auto apply_channel_defaults = [](RDK::TProjectChannelConfig& ch) {
        ch.ModelMode = 3;
        ch.ClassName = "Model";
        ch.CalculationMode = 1;
        ch.GlobalTimeStep = 2000;
        ch.DefaultTimeStep = 2000;
        ch.MinInterstepsInterval = 1;
        ch.InitAfterLoad = true;
        ch.ResetAfterLoad = true;
        ch.PredefinedStructure = 0;
    };

    for(int i = 0; i < num_channels; ++i)
    {
        apply_channel_defaults(cfg.ChannelsConfig[i]);
        if(args.contains("channels") && args.at("channels").is_array()
           && i < static_cast<int>(args.at("channels").size()))
        {
            const nlohmann::json& chj = args.at("channels")[i];
            if(chj.contains("class_name"))
                cfg.ChannelsConfig[i].ClassName = chj.at("class_name").get<std::string>();
            if(chj.contains("model_mode"))
                cfg.ChannelsConfig[i].ModelMode = chj.at("model_mode").get<int>();
            if(chj.contains("calculation_mode"))
                cfg.ChannelsConfig[i].CalculationMode = chj.at("calculation_mode").get<int>();
            if(chj.contains("global_time_step"))
                cfg.ChannelsConfig[i].GlobalTimeStep = chj.at("global_time_step").get<int>();
            if(chj.contains("default_time_step"))
                cfg.ChannelsConfig[i].DefaultTimeStep = chj.at("default_time_step").get<int>();
            if(chj.contains("min_intersteps_interval"))
                cfg.ChannelsConfig[i].MinInterstepsInterval =
                    chj.at("min_intersteps_interval").get<int>();
            if(chj.contains("init_after_load"))
                cfg.ChannelsConfig[i].InitAfterLoad = chj.at("init_after_load").get<bool>();
            if(chj.contains("reset_after_load"))
                cfg.ChannelsConfig[i].ResetAfterLoad = chj.at("reset_after_load").get<bool>();
            if(chj.contains("predefined_structure"))
                cfg.ChannelsConfig[i].PredefinedStructure =
                    chj.at("predefined_structure").get<int>();
        }
    }

    return cfg;
}

bool URdkApplicationCommands::handleOpenProjectConflict(const std::string& if_open_project,
                                                        std::string& err) const
{
    if(!m_app || !m_app->GetProjectOpenFlag())
        return true;

    if(if_open_project == "close")
    {
        m_app->CloseProject();
        return true;
    }
    if(if_open_project == "save_and_close")
    {
        if(!m_app->SaveProject())
        {
            err = "SaveProject failed before close";
            return false;
        }
        m_app->CloseProject();
        return true;
    }
    if(if_open_project == "deny")
    {
        err = "Another configuration is open; close it or set if_open_project to close/save_and_close";
        return false;
    }
    err = "invalid if_open_project value";
    return false;
}

ApplicationCommandResult URdkApplicationCommands::createConfiguration(const nlohmann::json& args)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");

    const std::string if_open = args.value("if_open_project", std::string("deny"));
    std::string conflict_err;
    if(!handleOpenProjectConflict(if_open, conflict_err))
        return fail(DomainStatusCode::InvalidPropertyValue, conflict_err);

    std::string err;
    const std::string ini_path = resolveProjectIniPath(args, m_app, err);
    if(ini_path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, err);

    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(ini_path, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    RDK::TProjectConfig cfg = buildProjectConfigFromJson(args, m_app, err);
    if(!err.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, err);

    if(!m_app->CreateProject(ini_path, cfg))
        return fail(DomainStatusCode::IOError, "CreateProject failed for " + ini_path);

    ApplicationCommandResult r;
    r.status = {};
    r.resolved_configuration_path = ini_path;
    r.payload = {{"project_ini_path", ini_path},
                 {"project_loaded", true},
                 {"channel_count", m_app->GetNumChannels()}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    r.update_context = true;
    r.add_to_recent = args.value("add_to_recent", true);
    return r;
}

ApplicationCommandResult URdkApplicationCommands::loadConfiguration(const std::string& path,
                                                                    const std::string& if_open_project)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "configuration path is required");

    std::string conflict_err;
    if(!handleOpenProjectConflict(if_open_project, conflict_err))
        return fail(DomainStatusCode::InvalidPropertyValue, conflict_err);

    const std::string ini_path = ensureProjectIniPath(path);
    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(ini_path, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    RDK::TProjectLoadDiagnostics diagnostics;
    if(!m_app->OpenProject(ini_path, &diagnostics))
    {
        ApplicationCommandResult r =
            fail(DomainStatusCode::ProjectNotLoaded, "OpenProject failed for " + ini_path);
        r.payload["diagnostics"] = diagnosticsToJson(diagnostics);
        return r;
    }

    ApplicationCommandResult r;
    r.status = {};
    r.resolved_configuration_path = ini_path;
    r.payload = {{"configuration_path", ini_path},
                 {"project_loaded", true},
                 {"channel_count", m_app->GetNumChannels()},
                 {"diagnostics", diagnosticsToJson(diagnostics)}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    r.update_context = true;
    r.add_to_recent = true;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::saveConfiguration()
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");

    if(!m_app->SaveProject())
        return fail(DomainStatusCode::IOError, "SaveProject failed");

    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"saved", true}};
    r.presentation = LLMPresentationEffect::None;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::saveConfigurationAs(const std::string& path)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "configuration path is required");

    const std::string ini_path = ensureProjectIniPath(path);
    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(ini_path, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    if(!m_app->SaveProjectAs(ini_path))
        return fail(DomainStatusCode::IOError, "SaveProjectAs failed");

    ApplicationCommandResult r;
    r.status = {};
    r.resolved_configuration_path = ini_path;
    r.payload = {{"configuration_path", ini_path}, {"saved", true}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    r.update_context = true;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::closeConfiguration()
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");

    ApplicationCommandResult r;
    r.status = {};
    if(!m_app->GetProjectOpenFlag())
    {
        r.payload = {{"closed", true}, {"project_loaded", false}};
        return r;
    }

    m_app->CloseProject();
    r.payload = {{"closed", true}, {"project_loaded", false}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    r.update_context = true;
    r.project_closed = true;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::updateConfiguration(
    const nlohmann::json& project_config_json)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");

    std::string err;
    RDK::TProjectConfig cfg = buildProjectConfigFromJson(project_config_json, m_app, err);
    if(!err.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, err);

    if(!m_app->UpdateProject(cfg))
        return fail(DomainStatusCode::IOError, "UpdateProject failed");

    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"updated", true}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::validateConfigurationFile(
    const std::string& project_ini_path)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(project_ini_path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "configuration path is required");

    const std::string ini_path = ensureProjectIniPath(project_ini_path);
    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(ini_path, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    const RDK::TProjectLoadDiagnostics diagnostics = m_app->ValidateProject(ini_path);
    ApplicationCommandResult r;
    r.status = {};
    r.payload = diagnosticsToJson(diagnostics);
    r.presentation = LLMPresentationEffect::None;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::saveConfigurationMetadata()
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(!m_app->SaveProjectConfig())
        return fail(DomainStatusCode::IOError, "SaveProjectConfig failed");

    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"saved", true}};
    return r;
}

ApplicationCommandResult URdkApplicationCommands::reloadConfigurationParameters()
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");

    m_app->ReloadParameters();
    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"reloaded", true}};
    r.presentation = LLMPresentationEffect::DiagramRefresh;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::copyConfiguration(const std::string& dest_dir,
                                                                  bool switch_to_copy)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(dest_dir.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "destination directory is required");

    std::string dest = dest_dir;
    if(dest.back() != '/' && dest.back() != '\\')
        dest.push_back('/');

    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(dest, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    if(!m_app->CopyProject(dest))
        return fail(DomainStatusCode::IOError, "CopyProject failed");

    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"copied", true}, {"destination", dest}};

    if(switch_to_copy)
    {
        const std::string open_path = dest + m_app->GetProjectFileName();
        auto load_r = loadConfiguration(open_path, "close");
        if(!load_r.status.ok())
            return load_r;
        return load_r;
    }
    return r;
}

ApplicationCommandResult URdkApplicationCommands::renameConfiguration(
    const std::string& new_directory_path)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(new_directory_path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "new_directory_path is required");

    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(new_directory_path, m_app, path_err))
        return fail(DomainStatusCode::PolicyDenied, path_err);

    if(!m_app->RenameProject(new_directory_path))
        return fail(DomainStatusCode::IOError, "RenameProject failed");

    ApplicationCommandResult r;
    r.status = {};
    const std::string ini = joinPath(m_app->GetProjectPath(), m_app->GetProjectFileName());
    r.resolved_configuration_path = ini;
    r.payload = {{"renamed", true}, {"configuration_path", ini}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    r.update_context = true;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::startChannelCalculation(int channel_index)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(channel_index < -1 || channel_index >= m_app->GetNumChannels())
        return fail(DomainStatusCode::InvalidPropertyValue, "invalid channel_index");

    m_app->StartChannel(channel_index);
    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"channel_index", channel_index}, {"started", true}};
    r.presentation = LLMPresentationEffect::DiagramRefresh;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::pauseChannelCalculation(int channel_index)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(channel_index < -1 || channel_index >= m_app->GetNumChannels())
        return fail(DomainStatusCode::InvalidPropertyValue, "invalid channel_index");

    m_app->PauseChannel(channel_index);
    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"channel_index", channel_index}, {"paused", true}};
    r.presentation = LLMPresentationEffect::DiagramRefresh;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::resetChannelCalculation(int channel_index)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(channel_index < -1 || channel_index >= m_app->GetNumChannels())
        return fail(DomainStatusCode::InvalidPropertyValue, "invalid channel_index");

    m_app->ResetChannel(channel_index);
    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"channel_index", channel_index}, {"reset", true}};
    r.presentation = LLMPresentationEffect::FullShellRefresh;
    return r;
}

ApplicationCommandResult URdkApplicationCommands::stepChannelCalculation(int channel_index)
{
    if(!m_app)
        return fail(DomainStatusCode::NotInitialized, "Application not available");
    if(!m_app->GetProjectOpenFlag())
        return fail(DomainStatusCode::ProjectNotLoaded, "No configuration is open");
    if(channel_index < -1 || channel_index >= m_app->GetNumChannels())
        return fail(DomainStatusCode::InvalidPropertyValue, "invalid channel_index");

    m_app->StepChannel(channel_index);
    ApplicationCommandResult r;
    r.status = {};
    r.payload = {{"channel_index", channel_index}, {"stepped", true}};
    r.presentation = LLMPresentationEffect::DiagramRefresh;
    return r;
}

static std::string displayNameFromPath(const std::string& path)
{
    // Avoid Qt in the domain layer: keep it simple and stable.
    // Prefer file name (project.ini), otherwise last directory segment.
    const std::string normalized = path;
    const size_t last_slash = normalized.find_last_of("/\\");
    if(last_slash == std::string::npos)
        return normalized;
    return normalized.substr(last_slash + 1);
}

nlohmann::json URdkApplicationCommands::listRecentConfigurations() const
{
    std::vector<std::string> qsettings_paths;
    if(LLMServices::instance().isInitialized())
    {
        if(auto* sink = LLMServices::instance().presentationSink())
            qsettings_paths = sink->recentConfigurationPaths();
    }

    static const std::list<std::string> kEmptyHistory;
    const std::list<std::string>& history = m_app ? m_app->GetLastProjectsList() : kEmptyHistory;

    std::vector<std::string> merged;
    merged.reserve(qsettings_paths.size() + history.size());
    std::unordered_set<std::string> seen;

    auto pushNormalized = [&](const std::string& raw) {
        if(raw.empty())
            return;
        const std::string ini = ensureProjectIniPath(raw);
        if(ini.empty())
            return;
        if(seen.insert(ini).second)
            merged.push_back(ini);
    };

    for(const std::string& p : qsettings_paths)
        pushNormalized(p);
    for(const std::string& p : history)
        pushNormalized(p);

    nlohmann::json out;
    out["items"] = nlohmann::json::array();
    for(std::size_t i = 0; i < merged.size(); ++i)
    {
        const std::string& p = merged[i];
        out["items"].push_back({{"index", static_cast<int>(i + 1)},
                                {"path", p},
                                {"display_name", displayNameFromPath(p)}});
    }
    return out;
}

ApplicationCommandResult URdkApplicationCommands::openRecentConfiguration(
    int index_1based, const std::string& if_open_project)
{
    const nlohmann::json j = listRecentConfigurations();
    const nlohmann::json items = j.value("items", nlohmann::json::array());
    if(index_1based < 1 || index_1based > static_cast<int>(items.size()))
        return fail(DomainStatusCode::InvalidPropertyValue,
                    "recent configuration index is out of range");

    const std::string path = items.at(static_cast<std::size_t>(index_1based - 1)).value("path", "");
    return loadConfiguration(path, if_open_project);
}

ApplicationCommandResult URdkApplicationCommands::openRecentConfigurationByPath(
    const std::string& path, const std::string& if_open_project)
{
    if(path.empty())
        return fail(DomainStatusCode::InvalidPropertyValue, "recent configuration path is empty");
    return loadConfiguration(path, if_open_project);
}

} // namespace RDK::LLM
