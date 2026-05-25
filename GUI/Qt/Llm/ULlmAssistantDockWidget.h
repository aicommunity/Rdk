#ifndef RDK_ULLM_ASSISTANT_DOCK_WIDGET_H
#define RDK_ULLM_ASSISTANT_DOCK_WIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>

#include "../../../LLM/Core/LlmTypes.h"
#include "../UVisualControllerWidget.h"
#include "ULlmGuiContextBridge.h"

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
    void onContextChanged(const LLMGuiContext& ctx);
    void onOpenSettings();
    void onProviderChanged(int index);
    void refreshProviderBar();

private:
    RDK::LLM::LLMSessionContext buildSession(const LLMGuiContext& ctx) const;
    void runUserMessage(const QString& text);

    ULlmGuiContextBridge* m_bridge = nullptr;
    QComboBox* m_provider_combo = nullptr;
    QLabel* m_provider_status = nullptr;
    QPlainTextEdit* m_input = nullptr;
    QTextEdit* m_history = nullptr;
    QPushButton* m_send = nullptr;
    QPushButton* m_confirm = nullptr;
    QPushButton* m_reject = nullptr;
    QPushButton* m_execute_plan = nullptr;
    QPushButton* m_resume_plan = nullptr;
    QPushButton* m_rollback_plan = nullptr;
    QString m_pending_confirmation_id;
    QString m_pending_plan_id;
    bool m_plan_paused = false;
    LLMGuiContext m_last_ctx;
};

#endif
