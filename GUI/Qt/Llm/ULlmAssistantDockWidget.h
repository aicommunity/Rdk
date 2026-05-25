#ifndef RDK_ULLM_ASSISTANT_DOCK_WIDGET_H
#define RDK_ULLM_ASSISTANT_DOCK_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>
#include <QTextEdit>

#include "../../../LLM/Core/LlmTypes.h"
#include "../UVisualControllerWidget.h"
#include "ULlmGuiContextBridge.h"

namespace RDK::LLM {
struct LLMFinalResponse;
}

class QTimer;

class ULlmAssistantDockWidget : public UVisualControllerWidget {
    Q_OBJECT
public:
    ULlmAssistantDockWidget(QWidget* parent, RDK::UApplication* app, ULlmGuiContextBridge* bridge);

    void appendAssistantText(const QString& text);
    void setPendingConfirmation(const QString& confirmation_id, const QString& summary);
    void clearPendingConfirmation();
    void setPendingPlan(const QString& plan_id, const QString& summary);
    void setPausedPlan(const QString& plan_id, const QString& summary);
    void clearPendingPlan();

public slots:
    void onSendClicked();
    void onConfirmClicked();
    void onRejectClicked();
    void onExecutePlanClicked();
    void onResumePlanClicked();
    void onRollbackPlanClicked();
    void onCancelClicked();
    void onContextChanged(const LLMGuiContext& ctx);
    void onOpenSettings();
    void onProviderChanged(int index);
    void refreshProviderBar();
    void onStreamToken(const QString& token);
    void onStreamFinished(const RDK::LLM::LLMFinalResponse& resp);
    void beginAssistantStream();
    void applyGuiPreferences();
    void trySendFromShortcut();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    RDK::LLM::LLMSessionContext buildSession(const LLMGuiContext& ctx) const;
    void runUserMessage(const QString& text);
    void endAssistantStream();
    void setRequestInProgress(bool busy);
    void updateSendButtonLabel();

    ULlmGuiContextBridge* m_bridge = nullptr;
    QComboBox* m_provider_combo = nullptr;
    QLabel* m_provider_status = nullptr;
    QLabel* m_request_status = nullptr;
    QProgressBar* m_request_progress = nullptr;
    QPlainTextEdit* m_input = nullptr;
    QTextEdit* m_history = nullptr;
    QPushButton* m_send = nullptr;
    QPushButton* m_cancel = nullptr;
    QPushButton* m_confirm = nullptr;
    QPushButton* m_reject = nullptr;
    QPushButton* m_execute_plan = nullptr;
    QPushButton* m_resume_plan = nullptr;
    QPushButton* m_rollback_plan = nullptr;
    QString m_pending_confirmation_id;
    QString m_pending_plan_id;
    bool m_plan_paused = false;
    bool m_streaming_reply = false;
    bool m_stream_tokens_received = false;
    QTimer* m_confirmation_timer = nullptr;
    QShortcut* m_shortcut_ctrl_return = nullptr;
    QShortcut* m_shortcut_ctrl_enter = nullptr;
    bool m_enter_send_filter_active = false;
    LLMGuiContext m_last_ctx;
};

#endif
