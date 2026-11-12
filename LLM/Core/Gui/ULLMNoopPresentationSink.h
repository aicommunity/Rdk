#ifndef RDK_ULLM_NOOP_PRESENTATION_SINK_H
#define RDK_ULLM_NOOP_PRESENTATION_SINK_H

#include "ILLMPresentationSink.h"

namespace RDK::LLM {

class ULLMNoopPresentationSink : public ILLMPresentationSink {
public:
    void apply(const LLMPresentationEvent& event) override { (void)event; }
};

} // namespace RDK::LLM

#endif
