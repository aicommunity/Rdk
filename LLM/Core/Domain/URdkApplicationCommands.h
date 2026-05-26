#ifndef RDK_URDK_APPLICATION_COMMANDS_H
#define RDK_URDK_APPLICATION_COMMANDS_H

#include "../LlmTypes.h"
#include "URdkDomainAccess.h"
#include <nlohmann/json.hpp>

namespace RDK {
class UApplication;
struct TProjectConfig;
}

namespace RDK::LLM {

class URdkApplicationCommands {
public:
    explicit URdkApplicationCommands(RDK::UApplication* app);

    ApplicationCommandResult createConfiguration(const nlohmann::json& args);
    ApplicationCommandResult loadConfiguration(const std::string& path,
                                                 const std::string& if_open_project = "deny");
    ApplicationCommandResult saveConfiguration();
    ApplicationCommandResult saveConfigurationAs(const std::string& path);
    ApplicationCommandResult closeConfiguration();

    ApplicationCommandResult updateConfiguration(const nlohmann::json& project_config_json);
    ApplicationCommandResult validateConfigurationFile(const std::string& project_ini_path);
    ApplicationCommandResult saveConfigurationMetadata();
    ApplicationCommandResult reloadConfigurationParameters();
    ApplicationCommandResult copyConfiguration(const std::string& dest_dir, bool switch_to_copy);
    ApplicationCommandResult renameConfiguration(const std::string& new_directory_path);

    ApplicationCommandResult startChannelCalculation(int channel_index);
    ApplicationCommandResult pauseChannelCalculation(int channel_index);
    ApplicationCommandResult resetChannelCalculation(int channel_index);
    ApplicationCommandResult stepChannelCalculation(int channel_index);

    /// List recent configurations merged from GUI recent list + in-app history.
    /// Output shape:
    /// { "items": [ { "index": 1, "path": ".../project.ini", "display_name": "..." }, ... ] }
    nlohmann::json listRecentConfigurations() const;

    /// Open recent configuration by 1-based index.
    ApplicationCommandResult openRecentConfiguration(int index_1based,
                                                      const std::string& if_open_project = "close");

    /// Open recent configuration by resolved ini/directory path.
    ApplicationCommandResult openRecentConfigurationByPath(const std::string& path,
                                                             const std::string& if_open_project = "close");

    static std::string resolveProjectIniPath(const nlohmann::json& args, RDK::UApplication* app,
                                             std::string& err);
    static std::string resolveConfigurationPathFromArgs(const nlohmann::json& args, std::string& err);
    static std::string ensureProjectIniPath(const std::string& path);
    static RDK::TProjectConfig buildProjectConfigFromJson(const nlohmann::json& args,
                                                          RDK::UApplication* app,
                                                          std::string& err);
    static std::string resolveDefaultConfigsRoot(RDK::UApplication* app);

    DomainSessionInfo sessionInfo() const;

private:
    ApplicationCommandResult fail(DomainStatusCode code, const std::string& message) const;
    bool handleOpenProjectConflict(const std::string& if_open_project, std::string& err) const;

    RDK::UApplication* m_app;
};

} // namespace RDK::LLM

#endif
