#ifndef RDK_ULLM_PRESENTATION_SCOPE_GUARD_H
#define RDK_ULLM_PRESENTATION_SCOPE_GUARD_H

#include "../LlmTypes.h"
#include "ILLMPresentationSink.h"

namespace RDK::LLM {

/// Temporarily navigates the host diagram to pinned scope for a write invoke, then restores.
class ULLMPresentationScopeGuard {
public:
    ULLMPresentationScopeGuard(ILLMPresentationSink* sink, const LLMGuiContextSnapshot& pin,
                               int channel_index, bool enabled);
    ~ULLMPresentationScopeGuard();

    ULLMPresentationScopeGuard(const ULLMPresentationScopeGuard&) = delete;
    ULLMPresentationScopeGuard& operator=(const ULLMPresentationScopeGuard&) = delete;

private:
    ILLMPresentationSink* m_sink = nullptr;
    std::string m_restore_token;
    bool m_active = false;
};

} // namespace RDK::LLM

#endif
