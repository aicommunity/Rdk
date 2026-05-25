#include "ULlmAssistantDockWidget.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Orchestrator/ULLMAgentOrchestrator.h"
#include "../../../LLM/Core/Settings/ULLMProviderAuth.h"
#include "LlmGuiBootstrap.h"

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
    m_confirm = new QPushButton(tr("Apply"), this);
    m_reject = new QPushButton(tr("Reject"), this);
    m_confirm->setVisible(false);
    m_reject->setVisible(false);
    row->addWidget(m_send);
    row->addWidget(m_confirm);
    row->addWidget(m_reject);
    layout->addLayout(row);

    connect(m_provider_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ULlmAssistantDockWidget::onProviderChanged);
    connect(settings_btn, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onOpenSettings);
    connect(m_send, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onSendClicked);
    connect(m_confirm, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onConfirmClicked);
    connect(m_reject, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRejectClicked);
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

void ULlmAssistantDockWidget::runUserMessage(const QString& text)
{
    const LLMGuiContext ctx = m_bridge ? m_bridge->currentContext() : m_last_ctx;
    RDK::LLM::LLMRequestEnvelope req;
    req.session_id = "gui-session";
    req.trace_id = "gui-trace";
    req.user_text = text.toStdString();
    req.session = buildSession(ctx);
    req.provider_profile = RDK::LLM::LLMServices::instance().activeProviderProfile();

    auto future = QtConcurrent::run([req]() {
        return RDK::LLM::LLMServices::instance().orchestrator().handleUserMessage(req);
    });
    auto* watcher = new QFutureWatcher<RDK::LLM::LLMFinalResponse>(this);
    connect(watcher, &QFutureWatcher<RDK::LLM::LLMFinalResponse>::finished, this,
            [this, watcher]() {
                const RDK::LLM::LLMFinalResponse resp = watcher->result();
                watcher->deleteLater();
                if(!resp.ok)
                {
                    appendAssistantText(QString::fromStdString("Error: " + resp.error));
                    return;
                }
                if(resp.pending_confirmation)
                {
                    setPendingConfirmation("pending", QString::fromStdString(resp.text));
                    return;
                }
                if(resp.needs_entity_clarification)
                {
                    appendAssistantText(tr("<b>Clarification needed</b>"));
                    appendAssistantText(QString::fromStdString(resp.text));
                    return;
                }
                appendAssistantText(QString::fromStdString(resp.text));
            });
    watcher->setFuture(future);
}

void ULlmAssistantDockWidget::onConfirmClicked()
{
    if(m_pending_confirmation_id.isEmpty())
        return;
    RDK::LLM::LLMServices::instance().orchestrator().confirmPending("gui-session",
                                                                     m_pending_confirmation_id.toStdString());
    clearPendingConfirmation();
    appendAssistantText(tr("Change applied."));
}

void ULlmAssistantDockWidget::onRejectClicked()
{
    RDK::LLM::LLMServices::instance().orchestrator().rejectPending("gui-session");
    clearPendingConfirmation();
    appendAssistantText(tr("Change rejected."));
}
