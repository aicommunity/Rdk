#ifndef RDK_ULLM_CHAT_HISTORY_DIALOG_H
#define RDK_ULLM_CHAT_HISTORY_DIALOG_H

#include <optional>

#include <QDialog>
#include <QString>

class ULlmChatHistoryArchive;

class ULlmChatHistoryDialog : public QDialog {
    Q_OBJECT
public:
    enum class Action { None, Open, Continue };

    struct Result {
        Action action = Action::None;
        QString chat_file;
        QString session_id;
    };

    static std::optional<Result> run(QWidget* parent, ULlmChatHistoryArchive& archive,
                                     const QString& sessions_dir);

private:
    explicit ULlmChatHistoryDialog(QWidget* parent, ULlmChatHistoryArchive& archive,
                                   const QString& sessions_dir);

    void populateTable();
    std::optional<Result> selectedResult(Action action) const;

    ULlmChatHistoryArchive& m_archive;
    QString m_sessions_dir;
    class QTableWidget* m_table = nullptr;
    std::optional<Result> m_result;
};

#endif
