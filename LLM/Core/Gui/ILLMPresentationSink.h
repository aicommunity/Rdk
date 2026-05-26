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

    /// Run UApplication-backed commands on the host UI thread when a GUI sink is present.
    virtual ApplicationCommandResult invokeHostSynchronized(
        const std::function<ApplicationCommandResult()>& run)
    {
        return run();
    }
};

} // namespace RDK::LLM

#endif
