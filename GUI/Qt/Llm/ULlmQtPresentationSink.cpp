#include "ULlmQtPresentationSink.h"

#include "../UGEngineControlWidget.h"
#include "ULlmGuiContextBridge.h"

#include <QSettings>
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

std::vector<std::string> ULlmQtPresentationSink::recentConfigurationPaths() const
{
    // Must be GUI-thread safe: Qt's QSettings isn't guaranteed thread-safe.
    // Tool execution is routed to host thread via invokeHostSynchronized.
    QSettings settings("NeuroModeler", "NeuroModeler");
    const QStringList paths = settings.value("RecentConfigs").toStringList();
    std::vector<std::string> out;
    out.reserve(paths.size());
    for(const QString& p : paths)
        out.push_back(p.toStdString());
    return out;
}

nlohmann::json ULlmQtPresentationSink::listLlmUiPanelsState() const
{
    if(!m_host)
        return nlohmann::json::object();
    if(QThread::currentThread() == thread())
        return m_host->listLlmUiPanelsState();

    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        m_pending_host_list_result = nlohmann::json::object();
        m_pending_host_list_run = [this]() {
            return m_host ? m_host->listLlmUiPanelsState() : nlohmann::json::object();
        };
    }

    const int timeout_ms = defaultInvokeTimeoutMs();
    auto fut = std::async(std::launch::async, [this]() {
        QMetaObject::invokeMethod(this, "runHostListUiPanelsOnGuiThread",
                                  Qt::BlockingQueuedConnection);
    });
    if(fut.wait_for(std::chrono::milliseconds(timeout_ms)) != std::future_status::ready)
        return nlohmann::json::object();
    fut.get();

    std::lock_guard<std::mutex> lock(m_host_mu);
    return m_pending_host_list_result;
}

RDK::LLM::ApplicationCommandResult ULlmQtPresentationSink::invokeHostSynchronized(
    const std::function<RDK::LLM::ApplicationCommandResult()>& run)
{
    if(QThread::currentThread() == thread())
        return run();

    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        m_pending_host_run = run;
    }

    const int timeout_ms = defaultInvokeTimeoutMs();
    auto fut = std::async(std::launch::async, [this]() {
        QMetaObject::invokeMethod(this, "runHostCommandOnGuiThread", Qt::BlockingQueuedConnection);
    });
    if(fut.wait_for(std::chrono::milliseconds(timeout_ms)) != std::future_status::ready)
    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        m_pending_host_run = nullptr;
        RDK::LLM::ApplicationCommandResult err;
        err.status.code = RDK::LLM::DomainStatusCode::IOError;
        err.status.message = "GUI host command timed out";
        return err;
    }
    fut.get();

    RDK::LLM::ApplicationCommandResult out;
    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        out = m_pending_host_result;
        m_pending_host_run = nullptr;
    }
    return out;
}

void ULlmQtPresentationSink::runHostCommandOnGuiThread()
{
    std::function<RDK::LLM::ApplicationCommandResult()> fn;
    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        fn = std::move(m_pending_host_run);
    }
    if(fn)
        m_pending_host_result = fn();
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

    if(m_pending.show_panel != RDK::LLM::LLMUiPanel::None)
    {
        if(m_pending.show_panel_visible)
            m_host->showLlmUiPanel(m_pending.show_panel);
        else
            m_host->showLlmUiPanel(RDK::LLM::LLMUiPanel::None);
    }

    if(m_pending.add_to_recent && !m_pending.configuration_ini_path.empty())
    {
        m_host->registerRecentConfigurationPath(
            QString::fromStdString(m_pending.configuration_ini_path));
    }
}

void ULlmQtPresentationSink::runHostListUiPanelsOnGuiThread()
{
    std::function<nlohmann::json()> fn;
    {
        std::lock_guard<std::mutex> lock(m_host_mu);
        fn = std::move(m_pending_host_list_run);
    }
    if(fn)
    {
        const nlohmann::json r = fn();
        std::lock_guard<std::mutex> lock(m_host_mu);
        m_pending_host_list_result = r;
    }
}
