#include "ULlmAssistantDockWidget.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Orchestrator/ULLMAgentOrchestrator.h"

ULlmAssistantDockWidget::ULlmAssistantDockWidget(QWidget* parent, RDK::UApplication* app,
                                               ULlmGuiContextBridge* bridge)
    : UVisualControllerWidget(parent, app)
    , m_bridge(bridge)
{
    Name = "LlmAssistant";
    ClassName = "ULlmAssistantDockWidget";

    auto* layout = new QVBoxLayout(this);
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

    connect(m_send, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onSendClicked);
    connect(m_confirm, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onConfirmClicked);
    connect(m_reject, &QPushButton::clicked, this, &ULlmAssistantDockWidget::onRejectClicked);
    if(m_bridge)
        connect(m_bridge, &ULlmGuiContextBridge::contextChanged, this,
                &ULlmAssistantDockWidget::onContextChanged);
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
    s.llm_write_enabled = true;
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
