#ifndef RDK_ILLM_PRESENTATION_SINK_H
#define RDK_ILLM_PRESENTATION_SINK_H

#include "../LlmTypes.h"

namespace RDK::LLM {

class ILLMPresentationSink {
public:
    virtual ~ILLMPresentationSink() = default;
    virtual void apply(const LLMPresentationEvent& event) = 0;
};

} // namespace RDK::LLM

#endif
