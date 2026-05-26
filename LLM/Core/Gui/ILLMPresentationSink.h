#ifndef RDK_ILLM_PRESENTATION_SINK_H
#define RDK_ILLM_PRESENTATION_SINK_H

#include "../LlmTypes.h"

#include <functional>

namespace RDK::LLM {

class ILLMPresentationSink {
public:
    virtual ~ILLMPresentationSink() = default;
    virtual void apply(const LLMPresentationEvent& event) = 0;

    /// Run UApplication-backed commands on the host UI thread when a GUI sink is present.
    virtual ApplicationCommandResult invokeHostSynchronized(
        const std::function<ApplicationCommandResult()>& run)
    {
        return run();
    }
};

} // namespace RDK::LLM

#endif
