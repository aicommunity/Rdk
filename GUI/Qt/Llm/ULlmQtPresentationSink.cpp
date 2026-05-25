#include "ULlmQtPresentationSink.h"

#include "../UGEngineControlWidget.h"
#include "ULlmGuiContextBridge.h"

#include <QMetaObject>
#include <QThread>

#include <chrono>
#include <cstdlib>
#include <future>

ULlmQtPresentationSink::ULlmQtPresentationSink(UGEngineControlWidget* host,
                                                   ULlmGuiContextBridge* bridge,
                                                   QObject* parent)
    : QObject(parent)
    , m_host(host)
    , m_bridge(bridge)
{
}

int ULlmQtPresentationSink::defaultInvokeTimeoutMs()
{
    if(const char* env = std::getenv("NMSDK_LLM_PRESENTATION_TIMEOUT_MS"))
    {
        const int v = std::atoi(env);
        if(v > 0)
            return v;
    }
    return 30000;
}

void ULlmQtPresentationSink::apply(const RDK::LLM::LLMPresentationEvent& event)
{
    m_pending = event;
    if(QThread::currentThread() == thread())
    {
        applyOnGuiThread();
        return;
    }

    const int timeout_ms = defaultInvokeTimeoutMs();
    auto fut = std::async(std::launch::async, [this]() {
        QMetaObject::invokeMethod(this, "applyOnGuiThread", Qt::BlockingQueuedConnection);
    });
    if(fut.wait_for(std::chrono::milliseconds(timeout_ms)) != std::future_status::ready)
        return;
    fut.get();
}

void ULlmQtPresentationSink::applyOnGuiThread()
{
    if(!m_host)
        return;

    if(m_pending.update_context && m_bridge)
    {
        if(m_pending.project_closed)
            m_bridge->onProjectClosed();
        else if(!m_pending.configuration_ini_path.empty())
            m_bridge->onProjectLoaded(
                QString::fromStdString(m_pending.configuration_ini_path));
    }

    if(m_pending.effect == RDK::LLM::LLMPresentationEffect::FullShellRefresh)
        m_host->refreshLlmPresentationShell();
    else if(m_pending.effect == RDK::LLM::LLMPresentationEffect::DiagramRefresh)
        m_host->refreshLlmPresentationDiagram();

    if(m_pending.add_to_recent && !m_pending.configuration_ini_path.empty())
    {
        m_host->registerRecentConfigurationPath(
            QString::fromStdString(m_pending.configuration_ini_path));
    }
}
