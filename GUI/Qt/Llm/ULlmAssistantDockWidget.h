#ifndef RDK_ULLM_ASSISTANT_DOCK_WIDGET_H
#define RDK_ULLM_ASSISTANT_DOCK_WIDGET_H

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QShortcut>

#include "../../../LLM/Core/LlmTypes.h"
#include "../UVisualControllerWidget.h"
#include "ULlmChatHistoryPanel.h"
#include "ULlmGuiContextBridge.h"
#include "ULlmChatInputCompleter.h"

class ULlmChatHistoryArchive;

namespace RDK::LLM {
struct LLMFinalResponse;
}

class QTimer;

template<typename T>
class QFutureWatcher;

class ULlmAssistantDockWidget : public UVisualControllerWidget {
    Q_OBJECT
public:
    ULlmAssistantDockWidget(QWidget* parent, RDK::UApplication* app, ULlmGuiContextBridge* bridge);
    ~ULlmAssistantDockWidget() override;

    void appendAssistantText(const QString& text);
    void setPendingConfirmation(const QString& confirmation_id, const QString& summary);
    void clearPendingConfirmation();
    void setPendingPlan(const QString& plan_id, const QString& summary);
    void setPausedPlan(const QString& plan_id, const QString& summary);
    void clearPendingPlan();
    void startNewChat(const QString& system_note = QString());

public slots:
    void onSendClicked();
    void onConfirmClicked();
    void onRejectClicked();
    void onExecutePlanClicked();
    void onResumePlanClicked();
    void onRollbackPlanClicked();
    void onCancelClicked();
    void onContextChanged(const LLMGuiContext& ctx);
    void onProjectOpened(const QString& configuration_ini_path);
    void onProjectClosed();
    void onOpenSettings();
    void onProviderChanged(int index);
    void refreshProviderBar();
    void onStreamToken(const QString& token);
    void onThinkingToken(const QString& token);
    void onStreamFinished(const RDK::LLM::LLMFinalResponse& resp);
    void beginAssistantStream();
    void applyGuiPreferences();
    void trySendFromShortcut();
    void onOpenChatHistory();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    RDK::LLM::LLMSessionContext buildSession(const LLMGuiContext& ctx) const;
    std::string currentSessionId() const;
    void runUserMessage(const QString& text);
    void endAssistantStream();
    void setRequestInProgress(bool busy);
    void updateSendButtonLabel();
    void appendThinkingDetails(const QString& thinking);

    bool chatArchiveEnabled() const;
    void ensureChatArchive();
    void maybeStartArchiveFile();
    void archiveHtmlFragment(const QString& html);
    void finalizeActiveArchive();
    void setArchiveViewMode(bool read_only, const QString& banner_text = QString());
    void openArchivedChat(const QString& chat_file_path, const QString& session_id);
    void continueArchivedChat(const QString& chat_file_path, const QString& session_id);
    void openArchivedChatNow(const QString& chat_file_path, const QString& session_id);
    void continueArchivedChatNow(const QString& chat_file_path, const QString& session_id);
    void rebuildHistoryFromSession(const std::string& session_id);
    void restoreHitlFromSession(const std::string& session_id);
    bool orchestratorBusyForCurrentSession() const;
    void scheduleDeferredNewChat(const QString& system_note);
    void flushDeferredUiActions();
    void handleAsyncLlmResult(QFutureWatcher<RDK::LLM::LLMFinalResponse>* watcher,
                              const std::function<void(const RDK::LLM::LLMFinalResponse&)>& on_response);
    void handleAsyncLlmFinished(QFutureWatcher<RDK::LLM::LLMFinalResponse>* watcher);

    struct DeferredArchiveAction {
        enum class Kind { OpenReadOnly, Continue };
        Kind kind = Kind::OpenReadOnly;
        QString chat_file;
        QString session_id;
    };

    ULlmGuiContextBridge* m_bridge = nullptr;
    QComboBox* m_provider_combo = nullptr;
    QLabel* m_provider_status = nullptr;
    QLabel* m_context_budget = nullptr;
    QLabel* m_archive_banner = nullptr;
    QLabel* m_request_status = nullptr;
    QProgressBar* m_request_progress = nullptr;
    QPlainTextEdit* m_input = nullptr;
    ULlmChatInputCompleter* m_name_completer = nullptr;
    ULlmChatHistoryPanel* m_history = nullptr;
    QPushButton* m_send = nullptr;
    QPushButton* m_history_btn = nullptr;
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
    QString m_pending_assistant_archive;
    QString m_stream_thinking;
    bool m_thinking_details_appended = false;
    QTimer* m_confirmation_timer = nullptr;
    QShortcut* m_shortcut_ctrl_return = nullptr;
    QShortcut* m_shortcut_ctrl_enter = nullptr;
    bool m_enter_send_filter_active = false;
    LLMGuiContext m_last_ctx;
    QString m_session_id;
    std::unique_ptr<ULlmChatHistoryArchive> m_chat_archive;
    QString m_active_archive_path;
    bool m_archive_view_mode = false;
    std::optional<DeferredArchiveAction> m_deferred_archive_action;
    bool m_deferred_new_chat_pending = false;
    QString m_deferred_new_chat_note;
};

#endif
