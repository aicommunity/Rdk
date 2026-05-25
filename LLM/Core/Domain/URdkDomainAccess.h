#ifndef RDK_URDK_DOMAIN_ACCESS_H
#define RDK_URDK_DOMAIN_ACCESS_H

#include "../LlmTypes.h"
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
                                int max_components = 200) const;
    DomainStatus listRegisteredClassNames(std::vector<std::string>& out) const;
    DomainStatus findComponentByLongName(const std::string& long_name,
                                         nlohmann::json& out,
                                         int channel_index = 0) const;
    DomainStatus getComponentProperties(const std::string& long_name,
                                        nlohmann::json& out,
                                        int channel_index = 0) const;

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
                                   int channel_index);
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
