#ifndef RDK_URDK_DOMAIN_ACCESS_H
#define RDK_URDK_DOMAIN_ACCESS_H

#include "../LlmTypes.h"
#include "ULLMLinkIdentity.h"
#include <nlohmann/json.hpp>
#include <vector>

namespace RDK {
class UApplication;
class UEngine;
class UEnvironment;
class UStorage;
class UNet;
}

namespace RDK::LLM {

class ILLMPresentationSink;

struct DomainSessionInfo {
    bool engine_ready = false;
    bool project_loaded = false;
    int channel_count = 0;
};

class URdkApplicationCommands;

class URdkDomainAccess {
public:
    explicit URdkDomainAccess(RDK::UApplication* app);

    void setApplicationCommands(URdkApplicationCommands* commands) { m_commands = commands; }
    void setPresentationSink(ILLMPresentationSink* sink) { m_sink = sink; }

    RDK::UApplication* application() const { return m_app; }
    RDK::UEngine* engine() const;
    RDK::UEnvironment* environment(int channel_index = 0) const;
    RDK::UStorage* storage() const;

    DomainSessionInfo sessionInfo() const;

    DomainStatus listNetSnapshot(nlohmann::json& out, int channel_index = 0,
                                int max_components = 200,
                                const std::string& root_long_name = "") const;
    DomainStatus listModelLinks(nlohmann::json& out, int channel_index = 0,
                                const std::string& root_long_name = "",
                                int offset = 0, int limit = -1) const;
    DomainStatus linkExistsInModel(const LinkQuad& quad, int channel_index,
                                   const std::string& root_long_name,
                                   bool& out_exists) const;
    DomainStatus listRegisteredClassNames(std::vector<std::string>& out) const;
    DomainStatus listRegisteredClasses(nlohmann::json& out,
                                     const std::string& library_filter = "") const;
    DomainStatus findComponentByLongName(const std::string& long_name,
                                         nlohmann::json& out,
                                         int channel_index = 0) const;
    /// Resolve short name or partial path under parent (snapshot walk).
    DomainStatus resolveComponentLongName(const std::string& hint,
                                          int channel_index,
                                          std::string& out_long_name,
                                          const std::string& parent_scope = "") const;
    DomainStatus getComponentProperties(const std::string& long_name,
                                        nlohmann::json& out,
                                        int channel_index = 0,
                                        const std::vector<std::string>& property_names = {}) const;
    /// Published link endpoints for connect_components (engine lookup lists).
    DomainStatus listComponentPubPorts(const std::string& long_name,
                                       int channel_index,
                                       std::vector<std::string>& outputs,
                                       std::vector<std::string>& inputs) const;
    DomainStatus getComponentClassName(const std::string& long_name,
                                       int channel_index,
                                       std::string& out_class_name) const;

    DomainStatus addComponent(const std::string& class_name,
                              const std::string& parent_long_name,
                              const std::string& short_name,
                              int channel_index,
                              std::string& out_long_name);
    DomainStatus removeComponent(const std::string& long_name, int channel_index);
    DomainStatus connectComponents(const std::string& from_long_name,
                                   const std::string& from_property,
                                   const std::string& to_long_name,
                                   const std::string& to_property,
                                   int channel_index,
                                   bool* already_existed_out = nullptr);
    DomainStatus breakComponentLink(const std::string& from_long_name,
                                    const std::string& from_property,
                                    const std::string& to_long_name,
                                    const std::string& to_property,
                                    int channel_index);
    DomainStatus loadProject(const std::string& path);
    DomainStatus saveProject(const std::string& path_optional);
    DomainStatus getPropertyValue(const std::string& long_name,
                                  const std::string& property_name,
                                  int channel_index,
                                  std::string& out_value,
                                  bool& found) const;
    DomainStatus setProperty(const std::string& long_name,
                             const std::string& property_name,
                             const std::string& value,
                             int channel_index,
                             std::string* previous_value_out = nullptr);
    DomainStatus validateProjectDryRun(std::vector<std::string>& warnings) const;

private:
    RDK::UApplication* m_app = nullptr;
    URdkApplicationCommands* m_commands = nullptr;
    ILLMPresentationSink* m_sink = nullptr;
};

} // namespace RDK::LLM

#endif
