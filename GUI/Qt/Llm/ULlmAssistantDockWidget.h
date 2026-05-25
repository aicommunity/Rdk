#ifndef RDK_ULLM_ASSISTANT_DOCK_WIDGET_H
#define RDK_ULLM_ASSISTANT_DOCK_WIDGET_H

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

public slots:
    void onSendClicked();
    void onConfirmClicked();
    void onRejectClicked();
    void onContextChanged(const LLMGuiContext& ctx);

private:
    RDK::LLM::LLMSessionContext buildSession(const LLMGuiContext& ctx) const;
    void runUserMessage(const QString& text);

    ULlmGuiContextBridge* m_bridge = nullptr;
    QPlainTextEdit* m_input = nullptr;
    QTextEdit* m_history = nullptr;
    QPushButton* m_send = nullptr;
    QPushButton* m_confirm = nullptr;
    QPushButton* m_reject = nullptr;
    QString m_pending_confirmation_id;
    LLMGuiContext m_last_ctx;
};

#endif
