#include "ULlmAssistantDockWidget.h"

#include "LlmGuiBootstrap.h"
#include "ULlmChatHistoryArchive.h"
#include "ULlmChatHistoryDialog.h"

#include <QDateTime>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTimer>
#include <QUuid>
#include <QtConcurrent/QtConcurrent>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QProgressBar>
#include <QTextCursor>
#include <QVBoxLayout>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Observability/ULLMToolTrace.h"
#include "../../../LLM/Core/Orchestrator/ULLMAgentOrchestrator.h"
#include "../../../LLM/Core/Orchestrator/ULLMWorkflowState.h"
#include "../../../LLM/Core/Policy/ULLMPolicyLimits.h"
#include "../../../LLM/Core/Session/ULLMConversationStore.h"
#include "../../../LLM/Core/Settings/ULLMProviderAuth.h"
#include "../UGEngineControlWidget.h"

#include <rdk_init.h>

#include <cstdlib>
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

namespace {

fs::path toArchivePath(const QString& path)
{
    return fs::path(path.toStdString());
}

void appendToolTraceToHistory(QTextEdit* history,
                              const std::function<void(const QString&)>& archive_fn,
                              const RDK::LLM::LLMFinalResponse& resp)
{
    if(resp.tool_trace.empty())
        return;
    const std::string html = RDK::LLM::formatTurnToolTraceHtml(resp.tool_trace);
    if(html.empty())
        return;
    const QString qhtml = QString::fromStdString(html);
    history->append(qhtml);
    if(archive_fn)
        archive_fn(qhtml);
}

RDK::LLM::LLMGuiContextSnapshot guiSnapshotFromContext(const LLMGuiContext& ctx)
{
    RDK::LLM::LLMGuiContextSnapshot snap;
    snap.channel_index = ctx.channel_index;
    snap.project_xml_path = ctx.project_xml_path.toStdString();
    snap.focused_component_long_name = ctx.focused_component_long_name.toStdString();
    snap.focused_class_name = ctx.focused_class_name.toStdString();
    snap.diagram_scope_long_name = ctx.diagram_scope_long_name.toStdString();
    snap.snapshot_fingerprint = ctx.snapshot_fingerprint;
    if(const char* cur = Env_GetCurrentComponentName())
        snap.current_component_long_name = cur;
    if(const char* cur_id = Env_GetCurrentComponentId())
        snap.current_component_id = cur_id;
    if(snap.current_component_long_name.empty())
        snap.current_component_long_name = snap.focused_component_long_name;
    return snap;
}

QString formatContextBudgetLabel(const RDK::LLM::LLMFinalResponse& resp)
{
    if(resp.context_messages_chars == 0 && resp.context_ephemeral_chars == 0)
        return {};
    const auto scale = [](const std::size_t chars) -> QString {
        if(chars < 1024)
            return QString::number(chars) + QLatin1Char('c');
        return QString::number(chars / 1024) + QLatin1String("k");
    };
    QString label = QObject::tr("Context: %1 history + %2 hints")
                        .arg(scale(resp.context_messages_chars))
                        .arg(scale(resp.context_ephemeral_chars));
    if(resp.context_compacted)
        label += QObject::tr(" (compacted)");
    return label;
}

QString rollbackStatusMessage(ULlmAssistantDockWidget* dock, const std::string& status)
{
    if(status == "rolled_back")
        return dock->tr("Plan rolled back successfully.");
    if(status == "rolled_back_nothing_to_compensate")
        return dock->tr("Nothing to roll back (no compensating actions recorded).");
    if(status == "partial_rollback")
        return dock->tr("Rollback incomplete — review the schema and audit log.");
    if(status == "rollback_failed")
        return dock->tr("Rollback failed — check the audit log.");
    return {};
}

void appendRollbackStatusIfPresent(ULlmAssistantDockWidget* dock,
                                   const RDK::LLM::LLMFinalResponse& resp)
{
    if(resp.rollback_status.empty())
        return;
    const QString msg = rollbackStatusMessage(dock, resp.rollback_status);
    if(msg.isEmpty())
        return;
    dock->appendAssistantText(dock->tr("<b>[Rollback]</b> %1").arg(msg));
}

QString formatMessageForHistory(const RDK::LLM::LLMMessage& msg)
{
    using RDK::LLM::LLMMessage;
    switch(msg.role)
    {
    case LLMMessage::Role::User:
        return QString("<p><b>You:</b> %1</p>")
            .arg(QString::fromStdString(msg.content).toHtmlEscaped());
    case LLMMessage::Role::Assistant: {
        const QString body = QString::fromStdString(msg.content);
        if(body.contains("<b>") || body.contains("<p>") || body.contains("<i>"))
            return QString("<p><b>Assistant:</b> %1</p>").arg(body);
        return QString("<p><b>Assistant:</b> %1</p>").arg(body.toHtmlEscaped());
    }
    case LLMMessage::Role::Tool: {
        const QString name =
            msg.tool_name ? QString::fromStdString(*msg.tool_name) : QStringLiteral("tool");
        return QString("<p><i>[Tool: %1]</i></p>").arg(name.toHtmlEscaped());
    }
    case LLMMessage::Role::System:
        if(msg.content.empty())
            return {};
        return QString("<p><i>%1</i></p>")
            .arg(QString::fromStdString(msg.content).toHtmlEscaped());
    }
    return {};
}

} // namespace

ULlmAssistantDockWidget::~ULlmAssistantDockWidget() = default;

ULlmAssistantDockWidget::ULlmAssistantDockWidget(QWidget* parent, RDK::UApplication* app,
                                               ULlmGuiContextBridge* bridge)
    : UVisualControllerWidget(parent, app)
    , m_bridge(bridge)
{
    Name = "LlmAssistant";
    ClassName = "ULlmAssistantDockWidget";

    auto* layout = new QVBoxLayout(this);

    auto* top_row = new QHBoxLayout();
    m_provider_combo = new QComboBox(this);
    auto* settings_btn = new QPushButton(tr("Settings..."), this);
    auto* new_chat_btn = new QPushButton(tr("New chat"), this);
    m_history_btn = new QPushButton(tr("History..."), this);
    m_provider_status = new QLabel(this);
    m_context_budget = new QLabel(this);
    m_context_budget->setObjectName(QStringLiteral("llmContextBudget"));
    m_context_budget->setStyleSheet(QStringLiteral("color: palette(mid); font-size: 11px;"));
    m_archive_banner = new QLabel(this);
    m_archive_banner->setVisible(false);
    m_archive_banner->setWordWrap(true);
    m_archive_banner->setStyleSheet(QStringLiteral("background-color: #fff3cd; padding: 4px;"));
    top_row->addWidget(m_provider_combo, 1);
    top_row->addWidget(new_chat_btn);
    top_row->addWidget(m_history_btn);
    top_row->addWidget(settings_btn);
    layout->addLayout(top_row);
    layout->addWidget(m_provider_status);
    layout->addWidget(m_context_budget);
    layout->addWidget(m_archive_banner);

    m_history = new QTextEdit(this);
    m_history->setReadOnly(true);
    layout->addWidget(m_history, 1);

    m_request_status = new QLabel(this);
    m_request_status->setVisible(false);
    layout->addWidget(m_request_status);

    m_request_progress = new QProgressBar(this);
    m_request_progress->setVisible(false);
    m_request_progress->setRange(0, 0);
    m_request_progress->setTextVisible(false);
    m_request_progress->setMaximumHeight(4);
    layout->addWidget(m_request_progress);

    m_input = new QPlainTextEdit(this);
    layout->addWidget(m_input);

    auto* row = new QHBoxLayout();
    m_send = new QPushButton(tr("Send"), this);
    m_cancel = new QPushButton(tr("Cancel"), this);
    m_cancel->setVisible(false);
    m_confirm = new QPushButton(tr("Apply"), this);
    m_reject = new QPushButton(tr("Reject"), this);
    m_execute_plan = new QPushButton(tr("Run plan"), this);
    m_resume_plan = new QPushButton(tr("Resume plan"), this);
    m_rollback_plan = new QPushButton(tr("Rollback plan"), this);
    m_confirm->setVisible(false);
    m_reject->setVisible(false);
    m_execute_plan->setVisible(false);
    m_resume_plan->setVisible(false);
    m_rollback_plan->setVisible(false);
    row->addWidget(m_send);
    row->addWidget(m_cancel);
    row->addWidget(m_execute_plan);
    row->addWidget(m_resume_plan);
    row->addWidget(m_rollback_plan);
    row->addWidget(m_confirm);
    row->addWidget(m_reject);
    layout->addLayout(row);

    connect(m_provider_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ULlmAssistantDockWidget::onProviderChanged);
    connect(settings_btn, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onOpenSettings);
    connect(new_chat_btn, &QPushButton::clicked, this,
            [this]() { scheduleDeferredNewChat(tr("<i>New chat started.</i>")); });
    connect(m_history_btn, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onOpenChatHistory);
    if(!chatArchiveEnabled())
        m_history_btn->setVisible(false);
    connect(m_send, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onSendClicked);
    connect(m_cancel, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onCancelClicked);
    connect(m_confirm, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onConfirmClicked);
    connect(m_reject, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRejectClicked);
    connect(m_execute_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onExecutePlanClicked);
    connect(m_resume_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onResumePlanClicked);
    connect(m_rollback_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRollbackPlanClicked);
    if(m_bridge)
    {
        connect(m_bridge, &ULlmGuiContextBridge::contextChanged, this,
                &ULlmAssistantDockWidget::onContextChanged);
        connect(m_bridge, &ULlmGuiContextBridge::projectOpened, this,
                &ULlmAssistantDockWidget::onProjectOpened);
        connect(m_bridge, &ULlmGuiContextBridge::projectClosed, this,
                &ULlmAssistantDockWidget::onProjectClosed);
    }

    m_session_id = QStringLiteral("gui-%1")
                       .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    m_shortcut_ctrl_return =
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), m_input);
    m_shortcut_ctrl_enter = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), m_input);
    connect(m_shortcut_ctrl_return, &QShortcut::activated, this,
            &ULlmAssistantDockWidget::trySendFromShortcut);
    connect(m_shortcut_ctrl_enter, &QShortcut::activated, this,
            &ULlmAssistantDockWidget::trySendFromShortcut);

    refreshProviderBar();
    applyGuiPreferences();
}

void ULlmAssistantDockWidget::refreshProviderBar()
{
    m_provider_combo->blockSignals(true);
    m_provider_combo->clear();
    auto& store = RDK::LLM::LLMServices::instance().settings();
    int select = 0;
    int i = 0;
    for(const auto& profile : store.listProfiles())
    {
        m_provider_combo->addItem(QString::fromStdString(profile.profile_id),
                                  QString::fromStdString(profile.profile_id));
        if(profile.profile_id == store.runtime().active_profile_id)
            select = i;
        ++i;
    }
    m_provider_combo->setCurrentIndex(select);
    m_provider_combo->blockSignals(false);

    const auto active = store.activeProfile();
    const bool has_key = RDK::LLM::ULLMProviderAuth::hasApiKey(active, store.runtime());
    const auto preset = store.presetProfile(active.profile_id);
    const bool custom_endpoint =
        active.base_url != preset.base_url || active.model != preset.model;
    QString status;
    if(active.is_cloud)
        status = has_key ? tr("Cloud · key set") : tr("Cloud · key missing");
    else
        status = tr("Local");
    if(custom_endpoint)
        status += tr(" · custom endpoint");
    status += tr("\n%1 · %2").arg(QString::fromStdString(active.base_url))
                   .arg(QString::fromStdString(active.model));
    m_provider_status->setText(status);
}

void ULlmAssistantDockWidget::onOpenSettings()
{
    LlmGui::OpenProviderSettingsDialog(this, application);
    refreshProviderBar();
    applyGuiPreferences();
}

void ULlmAssistantDockWidget::onProviderChanged(int index)
{
    if(index < 0)
        return;
    auto& store = RDK::LLM::LLMServices::instance().settings();
    store.setActiveProfileId(m_provider_combo->currentData().toString().toStdString());
    store.save();
    RDK::LLM::LLMServices::instance().applyActiveProvider();
    refreshProviderBar();
}

void ULlmAssistantDockWidget::appendAssistantText(const QString& text)
{
    m_history->append(text);
    if(!m_streaming_reply)
        archiveHtmlFragment(text);
}

void ULlmAssistantDockWidget::setPendingConfirmation(const QString& confirmation_id,
                                                   const QString& summary)
{
    m_pending_confirmation_id = confirmation_id;
    m_confirm->setVisible(true);
    m_reject->setVisible(true);
    appendAssistantText(summary);

    if(!m_confirmation_timer)
        m_confirmation_timer = new QTimer(this);
    m_confirmation_timer->stop();
    m_confirmation_timer->setSingleShot(true);
    disconnect(m_confirmation_timer, nullptr, this, nullptr);
    const int ttl_ms = RDK::LLM::defaultPolicyLimits().confirmation_ttl_seconds * 1000;
    connect(m_confirmation_timer, &QTimer::timeout, this, [this]() {
        if(m_pending_confirmation_id.isEmpty())
            return;
        RDK::LLM::LLMServices::instance().orchestrator().rejectPending(currentSessionId());
        clearPendingConfirmation();
        appendAssistantText(tr("Confirmation expired."));
    });
    m_confirmation_timer->start(ttl_ms);
}

void ULlmAssistantDockWidget::clearPendingConfirmation()
{
    if(m_confirmation_timer)
        m_confirmation_timer->stop();
    m_pending_confirmation_id.clear();
    m_confirm->setVisible(false);
    m_reject->setVisible(false);
}

void ULlmAssistantDockWidget::setPendingPlan(const QString& plan_id, const QString& summary)
{
    m_plan_paused = false;
    m_pending_plan_id = plan_id;
    m_execute_plan->setVisible(true);
    m_resume_plan->setVisible(false);
    m_rollback_plan->setVisible(false);
    m_reject->setVisible(true);
    appendAssistantText(summary);

    UGEngineControlWidget* host = nullptr;
    for(QWidget* w = parentWidget(); w; w = w->parentWidget())
    {
        if(auto* eng = qobject_cast<UGEngineControlWidget*>(w))
        {
            host = eng;
            break;
        }
    }
    LlmGui::showPlanPreview(host, summary);
}

void ULlmAssistantDockWidget::setPausedPlan(const QString& plan_id, const QString& summary)
{
    m_plan_paused = true;
    m_pending_plan_id = plan_id;
    m_execute_plan->setVisible(false);
    m_resume_plan->setVisible(true);
    m_rollback_plan->setVisible(true);
    m_reject->setVisible(true);
    appendAssistantText(summary);

    UGEngineControlWidget* host = nullptr;
    for(QWidget* w = parentWidget(); w; w = w->parentWidget())
    {
        if(auto* eng = qobject_cast<UGEngineControlWidget*>(w))
        {
            host = eng;
            break;
        }
    }
    LlmGui::showPlanPreview(host, summary);
}

void ULlmAssistantDockWidget::clearPendingPlan()
{
    m_pending_plan_id.clear();
    m_plan_paused = false;
    m_execute_plan->setVisible(false);
    m_resume_plan->setVisible(false);
    m_rollback_plan->setVisible(false);
}

std::string ULlmAssistantDockWidget::currentSessionId() const
{
    return m_session_id.toStdString();
}

void ULlmAssistantDockWidget::startNewChat(const QString& system_note)
{
    if(orchestratorBusyForCurrentSession())
    {
        scheduleDeferredNewChat(system_note);
        return;
    }

    if(!m_archive_view_mode)
        finalizeActiveArchive();

    // Read-only archive view must not delete the JSON session (Continue relies on it).
    if(!m_archive_view_mode && !m_session_id.isEmpty()
       && RDK::LLM::LLMServices::instance().isInitialized())
        RDK::LLM::LLMServices::instance().orchestrator().discardSession(currentSessionId());

    m_session_id = QStringLiteral("gui-%1")
                         .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    m_active_archive_path.clear();
    m_archive_view_mode = false;
    setArchiveViewMode(false);
    m_history->clear();
    clearPendingConfirmation();
    clearPendingPlan();
    m_reject->setVisible(false);
    setRequestInProgress(false);
    endAssistantStream();
    m_pending_assistant_archive.clear();
    if(m_context_budget)
        m_context_budget->clear();

    if(!system_note.isEmpty())
    {
        m_history->append(system_note);
        // System banner is not persisted until the first user message opens a file.
    }

    if(RDK::LLM::LLMServices::instance().isInitialized() && application
       && application->GetProjectOpenFlag())
    {
        const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
        RDK::LLM::LLMServices::instance().orchestrator().seedSessionContext(
            currentSessionId(), buildSession(ctx), guiSnapshotFromContext(ctx));
    }
}

void ULlmAssistantDockWidget::onProjectOpened(const QString& configuration_ini_path)
{
    (void)configuration_ini_path;
    scheduleDeferredNewChat(tr("<i>New chat — project was loaded.</i>"));
}

void ULlmAssistantDockWidget::onProjectClosed()
{
    scheduleDeferredNewChat(tr("<i>New chat — project was closed.</i>"));
}

bool ULlmAssistantDockWidget::orchestratorBusyForCurrentSession() const
{
    if(!RDK::LLM::LLMServices::instance().isInitialized() || m_session_id.isEmpty())
        return false;
    return RDK::LLM::LLMServices::instance().orchestrator().isSessionBusy(currentSessionId());
}

void ULlmAssistantDockWidget::scheduleDeferredNewChat(const QString& system_note)
{
    if(orchestratorBusyForCurrentSession())
    {
        m_deferred_new_chat_pending = true;
        m_deferred_new_chat_note = system_note;
        return;
    }
    startNewChat(system_note);
}

void ULlmAssistantDockWidget::flushDeferredUiActions()
{
    if(orchestratorBusyForCurrentSession())
        return;

    if(m_deferred_archive_action)
    {
        const DeferredArchiveAction action = *m_deferred_archive_action;
        m_deferred_archive_action.reset();
        if(action.kind == DeferredArchiveAction::Kind::Continue)
            continueArchivedChatNow(action.chat_file, action.session_id);
        else
            openArchivedChatNow(action.chat_file, action.session_id);
    }

    if(!m_deferred_new_chat_pending)
        return;
    m_deferred_new_chat_pending = false;
    const QString note = m_deferred_new_chat_note;
    m_deferred_new_chat_note.clear();
    startNewChat(note);
}

void ULlmAssistantDockWidget::handleAsyncLlmResult(
    QFutureWatcher<RDK::LLM::LLMFinalResponse>* watcher,
    const std::function<void(const RDK::LLM::LLMFinalResponse&)>& on_response)
{
    struct FlushDeferred {
        ULlmAssistantDockWidget* dock;
        ~FlushDeferred() { dock->flushDeferredUiActions(); }
    } flush{this};

    RDK::LLM::LLMFinalResponse resp;
    try
    {
        resp = watcher->result();
    }
    catch(const std::exception& ex)
    {
        resp.ok = false;
        resp.error = ex.what();
    }
    catch(...)
    {
        resp.ok = false;
        resp.error = "Unknown error during LLM request.";
    }
    watcher->deleteLater();
    if(on_response)
        on_response(resp);
}

void ULlmAssistantDockWidget::handleAsyncLlmFinished(
    QFutureWatcher<RDK::LLM::LLMFinalResponse>* watcher)
{
    handleAsyncLlmResult(watcher, [this](const RDK::LLM::LLMFinalResponse& resp) {
        onStreamFinished(resp);
    });
}

RDK::LLM::LLMSessionContext ULlmAssistantDockWidget::buildSession(const LLMGuiContext& ctx) const
{
    RDK::LLM::LLMSessionContext s;
    s.session_id = currentSessionId();
    s.user_name = application ? application->GetUserName() : "";
    s.user_id = application ? application->GetUserId() : 0;
    s.project_loaded = application && application->GetProjectOpenFlag();
    s.active_channel_index = ctx.channel_index;
    const auto& runtime = RDK::LLM::LLMServices::instance().settings().runtime();
    s.llm_write_enabled = runtime.llm_write_enabled;
    s.auto_apply_writes = runtime.llm_write_enabled && runtime.llm_auto_apply_writes;
    s.autonomous_mode = runtime.autonomous_mode;
    s.autonomous_steps_taken = 0;
    s.allow_cloud_llm = runtime.allow_cloud_providers;

    if(RDK::LLM::LLMServices::instance().isInitialized())
    {
        const RDK::LLM::ConversationState* persisted =
            RDK::LLM::LLMServices::instance().conversationState(currentSessionId());
        if(persisted && persisted->last_session_context)
        {
            const RDK::LLM::LLMSessionContext& p = *persisted->last_session_context;
            s.llm_write_enabled = p.llm_write_enabled;
            s.auto_apply_writes = p.auto_apply_writes;
            s.autonomous_mode = p.autonomous_mode;
            s.autonomous_steps_taken = p.autonomous_steps_taken;
            s.allow_cloud_llm = p.allow_cloud_llm;
            s.allow_save = p.allow_save;
        }
    }

    return s;
}

void ULlmAssistantDockWidget::onContextChanged(const LLMGuiContext& ctx)
{
    m_last_ctx = ctx;
}

void ULlmAssistantDockWidget::onSendClicked()
{
    if(m_archive_view_mode)
        return;
    const QString text = m_input->toPlainText().trimmed();
    if(text.isEmpty())
        return;
    m_input->clear();
    const QString user_html = QString("<b>You:</b> %1").arg(text.toHtmlEscaped());
    m_history->append(user_html);
    maybeStartArchiveFile();
    archiveHtmlFragment(user_html);
    runUserMessage(text);
}

void ULlmAssistantDockWidget::beginAssistantStream()
{
    m_streaming_reply = true;
    m_stream_tokens_received = false;
    const QString header = QString("<b>%1:</b> ").arg(tr("Assistant"));
    m_history->append(header);
    m_pending_assistant_archive = header;
}

void ULlmAssistantDockWidget::endAssistantStream()
{
    m_streaming_reply = false;
    m_history->append(QString());
}

void ULlmAssistantDockWidget::setRequestInProgress(bool busy)
{
    m_send->setEnabled(!busy);
    m_input->setEnabled(!busy);
    m_cancel->setVisible(busy);
    m_request_status->setVisible(busy);
    m_request_progress->setVisible(busy);
    if(busy)
        m_request_status->setText(tr("Waiting for model response…"));
    else
        m_request_status->clear();
}

void ULlmAssistantDockWidget::updateSendButtonLabel()
{
    const auto mode = RDK::LLM::LLMServices::instance().settings().runtime().send_shortcut;
    if(mode == RDK::LLM::LLMSendShortcutMode::Enter)
        m_send->setText(tr("Send (Enter)"));
    else
        m_send->setText(tr("Send (Ctrl+Enter)"));
}

void ULlmAssistantDockWidget::trySendFromShortcut()
{
    if(!m_send || !m_send->isEnabled())
        return;
    onSendClicked();
}

void ULlmAssistantDockWidget::applyGuiPreferences()
{
    RDK::LLM::LLMServices::instance().settings().reload();
    const auto mode = RDK::LLM::LLMServices::instance().settings().runtime().send_shortcut;
    updateSendButtonLabel();

    if(mode == RDK::LLM::LLMSendShortcutMode::Enter)
    {
        m_input->setPlaceholderText(
            tr("Ask about the model or configuration… (Enter to send, Shift+Enter for new line)"));
        if(m_shortcut_ctrl_return)
            m_shortcut_ctrl_return->setEnabled(false);
        if(m_shortcut_ctrl_enter)
            m_shortcut_ctrl_enter->setEnabled(false);
        if(!m_enter_send_filter_active)
        {
            m_input->installEventFilter(this);
            m_enter_send_filter_active = true;
        }
    }
    else
    {
        m_input->setPlaceholderText(
            tr("Ask about the model or configuration… (Ctrl+Enter to send)"));
        if(m_shortcut_ctrl_return)
            m_shortcut_ctrl_return->setEnabled(true);
        if(m_shortcut_ctrl_enter)
            m_shortcut_ctrl_enter->setEnabled(true);
        if(m_enter_send_filter_active)
        {
            m_input->removeEventFilter(this);
            m_enter_send_filter_active = false;
        }
    }
}

bool ULlmAssistantDockWidget::eventFilter(QObject* watched, QEvent* event)
{
    if(watched != m_input || event->type() != QEvent::KeyPress)
        return UVisualControllerWidget::eventFilter(watched, event);

    auto* key_event = static_cast<QKeyEvent*>(event);
    if(key_event->key() != Qt::Key_Return && key_event->key() != Qt::Key_Enter)
        return UVisualControllerWidget::eventFilter(watched, event);
    if(key_event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier))
        return UVisualControllerWidget::eventFilter(watched, event);

    trySendFromShortcut();
    return true;
}

void ULlmAssistantDockWidget::onStreamToken(const QString& token)
{
    if(token.isEmpty())
        return;
    m_stream_tokens_received = true;
    m_pending_assistant_archive += token;
    QTextCursor cursor = m_history->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(token);
    m_history->setTextCursor(cursor);
    m_history->ensureCursorVisible();
}

void ULlmAssistantDockWidget::onStreamFinished(const RDK::LLM::LLMFinalResponse& resp)
{
    setRequestInProgress(false);
    if(m_context_budget)
        m_context_budget->setText(formatContextBudgetLabel(resp));
    if(m_streaming_reply)
    {
        if(!m_pending_assistant_archive.isEmpty())
            archiveHtmlFragment(m_pending_assistant_archive);
        m_pending_assistant_archive.clear();
        endAssistantStream();
    }

    appendRollbackStatusIfPresent(this, resp);

    appendToolTraceToHistory(m_history,
                             [this](const QString& fragment) { archiveHtmlFragment(fragment); },
                             resp);

    if(!resp.ok)
    {
        appendAssistantText(QString::fromStdString("Error: " + resp.error));
        return;
    }
    if(resp.pending_plan_execution)
    {
        setPendingPlan(QString::fromStdString(resp.pending_plan_id), QString::fromStdString(resp.text));
        return;
    }
    if(resp.pending_confirmation)
    {
        const QString cid = resp.pending_confirmation_id.empty()
                                ? QStringLiteral("pending")
                                : QString::fromStdString(resp.pending_confirmation_id);
        setPendingConfirmation(cid, QString::fromStdString(resp.text));
        return;
    }
    if(resp.can_resume_plan)
    {
        setPausedPlan(QString::fromStdString(resp.pending_plan_id), QString::fromStdString(resp.text));
        return;
    }
    if(resp.awaiting_user_input)
    {
        appendAssistantText(tr("<b>Question</b>"));
        appendAssistantText(QString::fromStdString(resp.text));
        if(resp.user_choice_options.is_array() && !resp.user_choice_options.empty())
        {
            int idx = 1;
            for(const auto& choice : resp.user_choice_options)
            {
                if(choice.is_string())
                    appendAssistantText(QStringLiteral("%1) %2").arg(idx++).arg(
                        QString::fromStdString(choice.get<std::string>())));
            }
        }
        return;
    }
    if(resp.needs_entity_clarification || resp.needs_tool_disambiguation)
    {
        appendAssistantText(tr("<b>Clarification needed</b>"));
        appendAssistantText(QString::fromStdString(resp.text));
        return;
    }
    if(resp.needs_argument_clarification)
    {
        appendAssistantText(QString::fromStdString(resp.text));
        return;
    }
    if(!m_stream_tokens_received)
    {
        if(!resp.text.empty())
            appendAssistantText(QString::fromStdString(resp.text));
        else if(resp.ok && resp.rollback_status.empty())
            appendAssistantText(tr("Done."));
    }
}

void ULlmAssistantDockWidget::onCancelClicked()
{
    RDK::LLM::LLMServices::instance().orchestrator().cancelSession(m_session_id.toStdString());
    m_pending_assistant_archive.clear();
    appendAssistantText(tr("<i>[Cancelled]</i>"));
    setRequestInProgress(false);
    endAssistantStream();
}

bool ULlmAssistantDockWidget::chatArchiveEnabled() const
{
    if(const char* disable = std::getenv("NMSDK_LLM_DISABLE_CHAT_ARCHIVE"))
        return !(disable[0] == '1' && disable[1] == '\0');
    return true;
}

void ULlmAssistantDockWidget::ensureChatArchive()
{
    if(!chatArchiveEnabled() || m_chat_archive)
        return;
    auto* ctx = RDK::LLM::LLMServices::instance().projectContext();
    if(!ctx)
        return;
    m_chat_archive = std::make_unique<ULlmChatHistoryArchive>(ctx->paths().bin_root);
}

void ULlmAssistantDockWidget::maybeStartArchiveFile()
{
    if(m_archive_view_mode || !chatArchiveEnabled())
        return;
    ensureChatArchive();
    if(!m_chat_archive || !m_chat_archive->isWritable() || !m_active_archive_path.isEmpty())
        return;

    ChatArchiveMeta meta;
    meta.session_id = currentSessionId();
    meta.provider_id = RDK::LLM::LLMServices::instance().activeProviderProfile().profile_id;
    if(m_bridge)
        meta.project_path = m_bridge->currentContext().project_xml_path.toStdString();
    meta.created_at_iso = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();

    if(const auto path = m_chat_archive->startNewChatFile(meta))
        m_active_archive_path = QString::fromStdString(path->string());
}

void ULlmAssistantDockWidget::archiveHtmlFragment(const QString& html)
{
    if(m_archive_view_mode || html.isEmpty() || !chatArchiveEnabled())
        return;
    ensureChatArchive();
    if(!m_chat_archive || m_active_archive_path.isEmpty() || !m_chat_archive->isWritable())
        return;
    m_chat_archive->appendHtmlFragment(toArchivePath(m_active_archive_path), html.toStdString());
}

void ULlmAssistantDockWidget::finalizeActiveArchive()
{
    if(!m_chat_archive || m_active_archive_path.isEmpty())
        return;
    m_chat_archive->finalizeChat(toArchivePath(m_active_archive_path));
    m_active_archive_path.clear();
}

void ULlmAssistantDockWidget::setArchiveViewMode(bool read_only, const QString& banner_text)
{
    m_archive_view_mode = read_only;
    if(m_archive_banner)
    {
        if(read_only)
        {
            m_archive_banner->setText(
                banner_text.isEmpty() ? tr("Archived chat (read-only)") : banner_text);
            m_archive_banner->setVisible(true);
        }
        else
        {
            m_archive_banner->clear();
            m_archive_banner->setVisible(false);
        }
    }
    if(m_input)
        m_input->setEnabled(!read_only);
    if(m_send)
        m_send->setEnabled(!read_only && !m_cancel->isVisible());
    if(read_only)
    {
        clearPendingConfirmation();
        clearPendingPlan();
        m_cancel->setVisible(false);
    }
}

void ULlmAssistantDockWidget::onOpenChatHistory()
{
    if(!chatArchiveEnabled())
        return;
    ensureChatArchive();
    if(!m_chat_archive)
        return;

    auto* ctx = RDK::LLM::LLMServices::instance().projectContext();
    if(!ctx)
        return;

    const QString sessions_dir =
        QString::fromStdString((ctx->paths().repository_root / "LLM" / "sessions").string());
    const auto result = ULlmChatHistoryDialog::run(this, *m_chat_archive, sessions_dir);
    if(!result)
        return;
    if(result->action == ULlmChatHistoryDialog::Action::Open)
        openArchivedChat(result->chat_file, result->session_id);
    else if(result->action == ULlmChatHistoryDialog::Action::Continue)
        continueArchivedChat(result->chat_file, result->session_id);
}

void ULlmAssistantDockWidget::openArchivedChat(const QString& chat_file_path,
                                               const QString& session_id)
{
    if(orchestratorBusyForCurrentSession())
    {
        m_deferred_archive_action =
            DeferredArchiveAction{DeferredArchiveAction::Kind::OpenReadOnly, chat_file_path,
                                  session_id};
        return;
    }
    openArchivedChatNow(chat_file_path, session_id);
}

void ULlmAssistantDockWidget::openArchivedChatNow(const QString& chat_file_path,
                                                  const QString& session_id)
{
    if(!m_chat_archive)
        return;

    finalizeActiveArchive();

    const std::string live_session = currentSessionId();
    const std::string viewed_session = session_id.toStdString();
    if(RDK::LLM::LLMServices::instance().isInitialized() && !live_session.empty()
       && live_session != viewed_session)
        RDK::LLM::LLMServices::instance().orchestrator().discardSession(live_session);

    m_session_id = session_id;
    m_active_archive_path.clear();
    m_history->clear();
    clearPendingConfirmation();
    clearPendingPlan();
    m_pending_assistant_archive.clear();
    endAssistantStream();

    const std::string body = m_chat_archive->loadChatBodyHtml(toArchivePath(chat_file_path));
    if(!body.empty())
        m_history->setHtml(QString::fromStdString(body));
    setArchiveViewMode(true);
}

void ULlmAssistantDockWidget::continueArchivedChat(const QString& chat_file_path,
                                                   const QString& session_id)
{
    if(orchestratorBusyForCurrentSession())
    {
        m_deferred_archive_action =
            DeferredArchiveAction{DeferredArchiveAction::Kind::Continue, chat_file_path,
                                  session_id};
        return;
    }
    continueArchivedChatNow(chat_file_path, session_id);
}

void ULlmAssistantDockWidget::continueArchivedChatNow(const QString& chat_file_path,
                                                    const QString& session_id)
{
    if(!m_chat_archive)
        return;

    setArchiveViewMode(false);
    finalizeActiveArchive();

    const std::string previous_live_session = currentSessionId();
    if(RDK::LLM::LLMServices::instance().isInitialized() && !previous_live_session.empty())
        RDK::LLM::LLMServices::instance().orchestrator().discardSession(previous_live_session);

    const std::string session_std = session_id.toStdString();
    if(!RDK::LLM::LLMServices::instance().orchestrator().tryResumeSession(session_std))
    {
        QMessageBox::warning(
            this, tr("Continue chat"),
            tr("Could not restore the conversation session. The session file may be missing or invalid."));
        startNewChat();
        return;
    }

    m_session_id = session_id;
    m_active_archive_path = chat_file_path;

    rebuildHistoryFromSession(session_std);
    restoreHitlFromSession(session_std);
}

void ULlmAssistantDockWidget::rebuildHistoryFromSession(const std::string& session_id)
{
    const RDK::LLM::ConversationState* state =
        RDK::LLM::LLMServices::instance().conversationState(session_id);
    if(!state)
    {
        m_history->clear();
        return;
    }

    QString html;
    for(const RDK::LLM::LLMMessage& msg : state->messages)
    {
        const QString block = formatMessageForHistory(msg);
        if(!block.isEmpty())
            html += block;
    }
    m_history->clear();
    if(html.isEmpty())
        return;
    m_history->setHtml(html);
}

void ULlmAssistantDockWidget::restoreHitlFromSession(const std::string& session_id)
{
    const RDK::LLM::ConversationState* state =
        RDK::LLM::LLMServices::instance().conversationState(session_id);
    if(!state)
        return;

    if(state->pending)
    {
        m_pending_confirmation_id = QString::fromStdString(state->pending->confirmation_id);
        m_confirm->setVisible(true);
        m_reject->setVisible(true);
        return;
    }

    if(state->pending_plan)
    {
        m_pending_plan_id = QString::fromStdString(state->pending_plan->plan_id);
        m_reject->setVisible(true);
        if(state->workflow_phase == RDK::LLM::LLMWorkflowPhase::TaskExecuting
           || state->workflow_phase == RDK::LLM::LLMWorkflowPhase::Executing)
        {
            m_plan_paused = true;
            m_execute_plan->setVisible(false);
            m_resume_plan->setVisible(true);
            m_rollback_plan->setVisible(true);
        }
        else
        {
            m_plan_paused = false;
            m_execute_plan->setVisible(true);
            m_resume_plan->setVisible(false);
            m_rollback_plan->setVisible(false);
        }
        return;
    }

    if(state->pending_tool_arguments)
    {
        m_history->append(
            tr("<p><i>More information is required to continue the pending tool action.</i></p>"));
    }
}

void ULlmAssistantDockWidget::runUserMessage(const QString& text)
{
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    RDK::LLM::LLMRequestEnvelope req;
    req.session_id = currentSessionId();
    req.trace_id = "gui-trace";
    req.user_text = text.toStdString();
    req.session = buildSession(ctx);
    req.gui = guiSnapshotFromContext(ctx);
    req.provider_profile = RDK::LLM::LLMServices::instance().activeProviderProfile();

    const auto profile = RDK::LLM::LLMServices::instance().activeProviderProfile();
    const bool can_stream = profile.kind != RDK::LLM::LLMProviderKind::Mock;

    setRequestInProgress(true);
    if(can_stream)
        QMetaObject::invokeMethod(this, "beginAssistantStream", Qt::QueuedConnection);

    QPointer<ULlmAssistantDockWidget> self(this);
    auto future = QtConcurrent::run([req, can_stream, self]() {
        RDK::LLM::LLMStreamHandlers stream;
        if(can_stream && self)
        {
            stream.on_token = [self](const std::string& token) {
                if(!self)
                    return;
                const QString qtok = QString::fromStdString(token);
                QMetaObject::invokeMethod(self, "onStreamToken", Qt::QueuedConnection,
                                        Q_ARG(QString, qtok));
            };
        }
        return RDK::LLM::LLMServices::instance().orchestrator().handleUserMessage(
            req, can_stream ? &stream : nullptr);
    });

    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() { handleAsyncLlmFinished(watcher); });
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onConfirmClicked()
{
    if(m_pending_confirmation_id.isEmpty())
        return;
    if(orchestratorBusyForCurrentSession())
    {
        appendAssistantText(tr("Error: %1")
                                .arg(QString::fromUtf8(
                                    RDK::LLM::ULLMAgentOrchestrator::sessionBusyErrorMessage())));
        return;
    }
    const RDK::LLM::LLMFinalResponse resp = RDK::LLM::LLMServices::instance().orchestrator().confirmPending(
        currentSessionId(), m_pending_confirmation_id.toStdString());
    clearPendingConfirmation();
    appendToolTraceToHistory(m_history,
                             [this](const QString& fragment) { archiveHtmlFragment(fragment); },
                             resp);
    appendAssistantText(resp.ok ? QString::fromStdString(resp.text)
                                : QString::fromStdString("Error: " + resp.error));
}

void ULlmAssistantDockWidget::onRejectClicked()
{
    RDK::LLM::LLMServices::instance().orchestrator().rejectPending(currentSessionId());
    clearPendingConfirmation();
    clearPendingPlan();
    m_reject->setVisible(false);
    appendAssistantText(tr("Change rejected."));
}

void ULlmAssistantDockWidget::onExecutePlanClicked()
{
    if(m_pending_plan_id.isEmpty())
        return;
    if(orchestratorBusyForCurrentSession())
    {
        appendAssistantText(tr("Error: %1")
                                .arg(QString::fromUtf8(
                                    RDK::LLM::ULLMAgentOrchestrator::sessionBusyErrorMessage())));
        return;
    }
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    const std::string session_id = currentSessionId();
    setRequestInProgress(true);
    auto future = QtConcurrent::run([session, session_id]() {
        return RDK::LLM::LLMServices::instance().orchestrator().confirmPlanExecution(
            session_id, "gui-plan-trace", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                handleAsyncLlmResult(watcher, [this](const RDK::LLM::LLMFinalResponse& resp) {
                    setRequestInProgress(false);
                    if(resp.can_resume_plan)
                        setPausedPlan(QString::fromStdString(resp.pending_plan_id),
                                      QString::fromStdString(resp.text));
                    else
                    {
                        clearPendingPlan();
                        m_reject->setVisible(false);
                    }
                    appendAssistantText(resp.ok ? QString::fromStdString(resp.text)
                                                : QString::fromStdString("Error: " + resp.error));
                });
            });
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onResumePlanClicked()
{
    if(m_pending_plan_id.isEmpty() || !m_plan_paused)
        return;
    if(orchestratorBusyForCurrentSession())
    {
        appendAssistantText(tr("Error: %1")
                                .arg(QString::fromUtf8(
                                    RDK::LLM::ULLMAgentOrchestrator::sessionBusyErrorMessage())));
        return;
    }
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    const std::string session_id = currentSessionId();
    setRequestInProgress(true);
    auto future = QtConcurrent::run([session, session_id]() {
        return RDK::LLM::LLMServices::instance().orchestrator().resumePlanExecution(
            session_id, "gui-plan-resume", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                handleAsyncLlmResult(watcher, [this](const RDK::LLM::LLMFinalResponse& resp) {
                    setRequestInProgress(false);
                    if(resp.can_resume_plan)
                        setPausedPlan(QString::fromStdString(resp.pending_plan_id),
                                      QString::fromStdString(resp.text));
                    else
                    {
                        clearPendingPlan();
                        m_reject->setVisible(false);
                    }
                    appendAssistantText(resp.ok ? QString::fromStdString(resp.text)
                                                : QString::fromStdString("Error: " + resp.error));
                });
            });
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onRollbackPlanClicked()
{
    if(m_pending_plan_id.isEmpty())
        return;
    if(orchestratorBusyForCurrentSession())
    {
        appendAssistantText(tr("Error: %1")
                                .arg(QString::fromUtf8(
                                    RDK::LLM::ULLMAgentOrchestrator::sessionBusyErrorMessage())));
        return;
    }
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    const std::string session_id = currentSessionId();
    setRequestInProgress(true);
    auto future = QtConcurrent::run([session, session_id]() {
        return RDK::LLM::LLMServices::instance().orchestrator().rollbackPlanExecution(
            session_id, "gui-plan-rollback", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                handleAsyncLlmResult(watcher, [this](const RDK::LLM::LLMFinalResponse& resp) {
                    setRequestInProgress(false);
                    clearPendingPlan();
                    m_reject->setVisible(false);
                    appendRollbackStatusIfPresent(this, resp);
                    if(!resp.text.empty())
                        appendAssistantText(QString::fromStdString(resp.text));
                    else if(!resp.ok)
                        appendAssistantText(QString::fromStdString("Error: " + resp.error));
                });
            });
    watcher->setFuture(future);
}
