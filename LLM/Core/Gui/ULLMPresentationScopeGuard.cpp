#include "ULLMPresentationScopeGuard.h"

#include "../Domain/ULLMCurrentComponentScope.h"

namespace RDK::LLM {

namespace {

std::string writeScopeLongName(const LLMGuiContextSnapshot& pin)
{
    const std::string diagram = readDiagramScopeLongName(&pin);
    if(!diagram.empty())
        return diagram;
    const CurrentComponentScope cur = readCurrentComponentScope(&pin);
    return cur.valid ? cur.long_name : std::string();
}

} // namespace

ULLMPresentationScopeGuard::ULLMPresentationScopeGuard(ILLMPresentationSink* sink,
                                                       const LLMGuiContextSnapshot& pin,
                                                       int channel_index, bool enabled)
    : m_sink(sink)
{
    if(!enabled || !m_sink)
        return;

    const std::string scope = writeScopeLongName(pin);
    if(scope.empty())
        return;

    const ApplicationCommandResult cap = m_sink->invokeHostSynchronized([&]() {
        ApplicationCommandResult r;
        m_restore_token = m_sink->captureNavigationToken();
        m_sink->navigateToDiagramScope(scope, channel_index);
        r.status = {};
        return r;
    });
    if(cap.status.ok())
        m_active = !m_restore_token.empty();
}

ULLMPresentationScopeGuard::~ULLMPresentationScopeGuard()
{
    if(!m_active || !m_sink || m_restore_token.empty())
        return;
    m_sink->invokeHostSynchronized([&]() {
        ApplicationCommandResult r;
        m_sink->restoreNavigationToken(m_restore_token);
        r.status = {};
        return r;
    });
}

} // namespace RDK::LLM
