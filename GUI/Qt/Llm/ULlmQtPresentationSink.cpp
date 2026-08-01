#include "ULlmQtPresentationSink.h"

#include "../UGEngineControlWidget.h"
#include "../UModernDiagramContainerWidget.h"
#include "../UEngineSelectionSync.h"
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

    if(event.effect == RDK::LLM::LLMPresentationEffect::DiagramRefresh)
    {
        if(m_diagramRefreshQueued)
            return;
        m_diagramRefreshQueued = true;
        QMetaObject::invokeMethod(this, "applyOnGuiThread", Qt::QueuedConnection);
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
        QMetaObject::invokeMethod(const_cast<ULlmQtPresentationSink*>(this),
                                  "runHostListUiPanelsOnGuiThread",
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
    {
        m_host->refreshLlmPresentationDiagram();
        m_diagramRefreshQueued = false;
    }

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

    if(m_pending.select_active_channel >= 0)
    {
        m_host->setLlmActiveChannel(m_pending.select_active_channel);
        if(m_bridge)
            m_bridge->onChannelChanged(m_pending.select_active_channel);
    }
}

std::string ULlmQtPresentationSink::captureNavigationToken() const
{
    if(!m_bridge)
        return {};
    const LLMGuiContext ctx = m_bridge->currentContext();
    nlohmann::json token;
    token["diagram_scope"] = ctx.diagram_scope_long_name.toStdString();
    token["channel_index"] = ctx.channel_index;
    return token.dump();
}

void ULlmQtPresentationSink::navigateToDiagramScope(const std::string& scope_long_name,
                                                    int channel_index)
{
    if(!m_host)
        return;
    if(channel_index >= 0)
    {
        m_host->setLlmActiveChannel(channel_index);
        if(m_bridge)
            m_bridge->onChannelChanged(channel_index);
    }
    UModernDiagramContainerWidget* container = m_host->modernDiagramContainer();
    if(!container)
        return;
    UModernDiagramWidget* diagram = container->modernDiagramWidget();
    if(!diagram)
        return;
    const QString name = QString::fromStdString(scope_long_name);
    diagram->SetComponentName(name);
    diagram->Reload();
    if(m_bridge)
        m_bridge->onDiagramScopeChanged(name.isEmpty() ? QStringLiteral("Model") : name);
}

void ULlmQtPresentationSink::restoreNavigationToken(const std::string& token)
{
    if(token.empty())
        return;
    const nlohmann::json j = nlohmann::json::parse(token, nullptr, false);
    if(!j.is_object())
        return;
    navigateToDiagramScope(j.value("diagram_scope", std::string()),
                           j.value("channel_index", 0));
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

nlohmann::json ULlmQtPresentationSink::watchAddSeries(const RDK::LLM::LLMWatchSeriesArgs& args)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload = m_host->llmWatchAddSeries(args.surface, args.mdi_id, args.tab_index,
                                              args.chart_index, args.channel_index,
                                              QString::fromStdString(args.long_name),
                                              QString::fromStdString(args.property_name), args.jx,
                                              args.jy);
        if(r.payload.value("ok", false) && args.surface == "window")
        {
            r.show_panel = RDK::LLM::LLMUiPanel::Watch;
            r.show_panel_visible = true;
        }
        if(!r.payload.value("ok", false))
        {
            r.status.code = RDK::LLM::DomainStatusCode::InvalidPropertyValue;
            r.status.message = r.payload.value("error", "watchAddSeries failed");
        }
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchListSeries(const RDK::LLM::LLMWatchSeriesArgs& args)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload =
            m_host->llmWatchListSeries(args.surface, args.mdi_id, args.tab_index, args.chart_index);
        if(!r.payload.value("ok", false))
        {
            r.status.code = RDK::LLM::DomainStatusCode::InvalidPropertyValue;
            r.status.message = r.payload.value("error", "watchListSeries failed");
        }
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}, {"items", nlohmann::json::array()}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchRemoveSeries(const RDK::LLM::LLMWatchSeriesArgs& args)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload = m_host->llmWatchRemoveSeries(
            args.surface, args.mdi_id, args.tab_index, args.chart_index, args.serie_index,
            QString::fromStdString(args.long_name), QString::fromStdString(args.property_name));
        if(!r.payload.value("ok", false))
        {
            r.status.code = RDK::LLM::DomainStatusCode::InvalidPropertyValue;
            r.status.message = r.payload.value("error", "watchRemoveSeries failed");
        }
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchClearSeries(const RDK::LLM::LLMWatchSeriesArgs& args)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload =
            m_host->llmWatchClearSeries(args.surface, args.mdi_id, args.tab_index, args.chart_index);
        if(!r.payload.value("ok", false))
        {
            r.status.code = RDK::LLM::DomainStatusCode::InvalidPropertyValue;
            r.status.message = r.payload.value("error", "watchClearSeries failed");
        }
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchMdiList()
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload = m_host->llmWatchMdiList();
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}, {"items", nlohmann::json::array()}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchMdiCreate(int grid_rows, int grid_cols,
                                                      const std::string& title)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host)
        {
            r.status.code = RDK::LLM::DomainStatusCode::NotInitialized;
            r.status.message = "Watch host unavailable";
            return r;
        }
        r.payload = m_host->llmWatchMdiCreate(grid_rows, grid_cols, QString::fromStdString(title));
        if(!r.payload.value("ok", false))
        {
            r.status.code = RDK::LLM::DomainStatusCode::IOError;
            r.status.message = r.payload.value("error", "watchMdiCreate failed");
        }
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchMdiFocus(int mdi_id)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host || !m_host->llmWatchMdiFocus(mdi_id))
        {
            r.status.code = RDK::LLM::DomainStatusCode::ComponentNotFound;
            r.status.message = "Watch MDI not found";
            r.payload = {{"ok", false}, {"error", r.status.message}};
            return r;
        }
        r.payload = {{"ok", true}, {"mdi_id", mdi_id}};
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}

nlohmann::json ULlmQtPresentationSink::watchMdiClose(int mdi_id)
{
    RDK::LLM::ApplicationCommandResult cmd = invokeHostSynchronized([&]() {
        RDK::LLM::ApplicationCommandResult r;
        if(!m_host || !m_host->llmWatchMdiClose(mdi_id))
        {
            r.status.code = RDK::LLM::DomainStatusCode::ComponentNotFound;
            r.status.message = "Watch MDI not found";
            r.payload = {{"ok", false}, {"error", r.status.message}};
            return r;
        }
        r.payload = {{"ok", true}, {"mdi_id", mdi_id}};
        return r;
    });
    if(!cmd.status.ok())
        return {{"ok", false}, {"error", cmd.status.message}};
    return cmd.payload;
}
