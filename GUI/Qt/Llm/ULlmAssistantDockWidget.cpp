#include "ULlmAssistantDockWidget.h"

#include "LlmGuiBootstrap.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QTextCursor>
#include <QVBoxLayout>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Orchestrator/ULLMAgentOrchestrator.h"
#include "../../../LLM/Core/Settings/ULLMProviderAuth.h"
#include "../UGEngineControlWidget.h"

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
    m_provider_status = new QLabel(this);
    top_row->addWidget(m_provider_combo, 1);
    top_row->addWidget(settings_btn);
    layout->addLayout(top_row);
    layout->addWidget(m_provider_status);

    m_history = new QTextEdit(this);
    m_history->setReadOnly(true);
    layout->addWidget(m_history, 1);

    m_input = new QPlainTextEdit(this);
    m_input->setPlaceholderText(tr("Ask about the model or configuration..."));
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
    connect(m_send, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onSendClicked);
    connect(m_cancel, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onCancelClicked);
    connect(m_confirm, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onConfirmClicked);
    connect(m_reject, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRejectClicked);
    connect(m_execute_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onExecutePlanClicked);
    connect(m_resume_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onResumePlanClicked);
    connect(m_rollback_plan, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRollbackPlanClicked);
    if(m_bridge)
        connect(m_bridge, &ULlmGuiContextBridge::contextChanged, this,
                &ULlmAssistantDockWidget::onContextChanged);
    refreshProviderBar();
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
}

void ULlmAssistantDockWidget::setPendingConfirmation(const QString& confirmation_id,
                                                   const QString& summary)
{
    m_pending_confirmation_id = confirmation_id;
    m_confirm->setVisible(true);
    m_reject->setVisible(true);
    appendAssistantText(summary);
}

void ULlmAssistantDockWidget::clearPendingConfirmation()
{
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

RDK::LLM::LLMSessionContext ULlmAssistantDockWidget::buildSession(const LLMGuiContext& ctx) const
{
    RDK::LLM::LLMSessionContext s;
    s.session_id = "gui-session";
    s.user_name = application ? application->GetUserName() : "";
    s.user_id = application ? application->GetUserId() : 0;
    s.project_loaded = application && application->GetProjectOpenFlag();
    s.active_channel_index = ctx.channel_index;
    const auto& runtime = RDK::LLM::LLMServices::instance().settings().runtime();
    s.llm_write_enabled = runtime.llm_write_enabled;
    s.allow_cloud_llm = runtime.allow_cloud_providers;
    return s;
}

void ULlmAssistantDockWidget::onContextChanged(const LLMGuiContext& ctx)
{
    m_last_ctx = ctx;
}

void ULlmAssistantDockWidget::onSendClicked()
{
    const QString text = m_input->toPlainText().trimmed();
    if(text.isEmpty())
        return;
    m_input->clear();
    m_history->append(QString("<b>You:</b> %1").arg(text.toHtmlEscaped()));
    runUserMessage(text);
}

void ULlmAssistantDockWidget::beginAssistantStream()
{
    m_streaming_reply = true;
    m_stream_tokens_received = false;
    m_history->append(QString("<b>%1:</b> ").arg(tr("Assistant")));
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
}

void ULlmAssistantDockWidget::onStreamToken(const QString& token)
{
    if(token.isEmpty())
        return;
    m_stream_tokens_received = true;
    QTextCursor cursor = m_history->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(token);
    m_history->setTextCursor(cursor);
    m_history->ensureCursorVisible();
}

void ULlmAssistantDockWidget::onStreamFinished(const RDK::LLM::LLMFinalResponse& resp)
{
    setRequestInProgress(false);
    if(m_streaming_reply)
        endAssistantStream();

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
        setPendingConfirmation("pending", QString::fromStdString(resp.text));
        return;
    }
    if(resp.can_resume_plan)
    {
        setPausedPlan(QString::fromStdString(resp.pending_plan_id), QString::fromStdString(resp.text));
        return;
    }
    if(resp.needs_entity_clarification)
    {
        appendAssistantText(tr("<b>Clarification needed</b>"));
        appendAssistantText(QString::fromStdString(resp.text));
        return;
    }
    if(!resp.text.empty() && !m_stream_tokens_received)
        appendAssistantText(QString::fromStdString(resp.text));
}

void ULlmAssistantDockWidget::onCancelClicked()
{
    RDK::LLM::LLMServices::instance().orchestrator().cancel();
    appendAssistantText(tr("[Cancelled]"));
    setRequestInProgress(false);
    endAssistantStream();
}

void ULlmAssistantDockWidget::runUserMessage(const QString& text)
{
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    RDK::LLM::LLMRequestEnvelope req;
    req.session_id = "gui-session";
    req.trace_id = "gui-trace";
    req.user_text = text.toStdString();
    req.session = buildSession(ctx);
    req.provider_profile = RDK::LLM::LLMServices::instance().activeProviderProfile();

    const auto profile = RDK::LLM::LLMServices::instance().activeProviderProfile();
    const bool can_stream = profile.kind != RDK::LLM::LLMProviderKind::EmbeddedLlama
                            && profile.kind != RDK::LLM::LLMProviderKind::Mock;

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
            [this, watcher]() {
                const RDK::LLM::LLMFinalResponse resp = watcher->result();
                watcher->deleteLater();
                onStreamFinished(resp);
            });
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onConfirmClicked()
{
    if(m_pending_confirmation_id.isEmpty())
        return;
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMFinalResponse resp = RDK::LLM::LLMServices::instance().orchestrator().confirmPending(
        "gui-session", m_pending_confirmation_id.toStdString());
    clearPendingConfirmation();
    appendAssistantText(resp.ok ? QString::fromStdString(resp.text)
                                : QString::fromStdString("Error: " + resp.error));
    (void)ctx;
}

void ULlmAssistantDockWidget::onRejectClicked()
{
    RDK::LLM::LLMServices::instance().orchestrator().rejectPending("gui-session");
    clearPendingConfirmation();
    clearPendingPlan();
    m_reject->setVisible(false);
    appendAssistantText(tr("Change rejected."));
}

void ULlmAssistantDockWidget::onExecutePlanClicked()
{
    if(m_pending_plan_id.isEmpty())
        return;
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    auto future = QtConcurrent::run([session]() {
        return RDK::LLM::LLMServices::instance().orchestrator().confirmPlanExecution(
            "gui-session", "gui-plan-trace", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                const RDK::LLM::LLMFinalResponse resp = watcher->result();
                watcher->deleteLater();
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
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onResumePlanClicked()
{
    if(m_pending_plan_id.isEmpty() || !m_plan_paused)
        return;
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    auto future = QtConcurrent::run([session]() {
        return RDK::LLM::LLMServices::instance().orchestrator().resumePlanExecution(
            "gui-session", "gui-plan-resume", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                const RDK::LLM::LLMFinalResponse resp = watcher->result();
                watcher->deleteLater();
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
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onRollbackPlanClicked()
{
    if(m_pending_plan_id.isEmpty())
        return;
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    const RDK::LLM::LLMSessionContext session = buildSession(ctx);
    auto future = QtConcurrent::run([session]() {
        return RDK::LLM::LLMServices::instance().orchestrator().rollbackPlanExecution(
            "gui-session", "gui-plan-rollback", session);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                const RDK::LLM::LLMFinalResponse resp = watcher->result();
                watcher->deleteLater();
                clearPendingPlan();
                m_reject->setVisible(false);
                appendAssistantText(QString::fromStdString(resp.text));
            });
    watcher->setFuture(future);
}
