#ifndef RDK_ILLM_PRESENTATION_SINK_H
#define RDK_ILLM_PRESENTATION_SINK_H

#include "../LlmTypes.h"

#include <functional>

namespace RDK::LLM {

class ILLMPresentationSink {
public:
    virtual ~ILLMPresentationSink() = default;
    virtual void apply(const LLMPresentationEvent& event) = 0;

    /// Read host-level recent configuration paths (most recent first).
    /// Used by LLM tools to implement `list_recent_configurations` and lifecycle "last".
    virtual std::vector<std::string> recentConfigurationPaths() const { return {}; }

    /// Read host UI panels state (id, title, visible).
    /// Used by LLM tools to implement `list_ui_panels`.
    virtual nlohmann::json listLlmUiPanelsState() const { return nlohmann::json::object(); }

    /// Run UApplication-backed commands on the host UI thread when a GUI sink is present.
    virtual ApplicationCommandResult invokeHostSynchronized(
        const std::function<ApplicationCommandResult()>& run)
    {
        return run();
    }

    /// Opaque host navigation state for diagram drill level (GUI sinks only).
    virtual std::string captureNavigationToken() const { return {}; }

    virtual void restoreNavigationToken(const std::string& token) { (void)token; }

    virtual void navigateToDiagramScope(const std::string& scope_long_name, int channel_index)
    {
        (void)scope_long_name;
        (void)channel_index;
    }
};

} // namespace RDK::LLM

#endif
