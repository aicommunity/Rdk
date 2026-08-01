#ifndef RDK_ULLM_CHAT_INPUT_COMPLETER_H
#define RDK_ULLM_CHAT_INPUT_COMPLETER_H

#include <QObject>
#include <QStringList>

class QPlainTextEdit;
class QCompleter;

namespace RDK {
class UApplication;
}

/// Tab-complete component long_names / class names / properties in LLM chat input.
class ULlmChatInputCompleter : public QObject {
    Q_OBJECT
public:
    explicit ULlmChatInputCompleter(QPlainTextEdit* input, RDK::UApplication* app,
                                    QObject* parent = nullptr);

    void setApplication(RDK::UApplication* app);
    void refreshDictionary(int channel_index = 0);
    /// Handle Tab / Shift+Tab. Returns true if consumed.
    bool handleTab(bool forward);

private:
    void applyCompletion(const QString& completion);
    QStringList currentSuggestions() const;

    QPlainTextEdit* m_input = nullptr;
    RDK::UApplication* m_app = nullptr;
    QCompleter* m_completer = nullptr;
    QStringList m_long_names;
    QStringList m_class_names;
    QStringList m_property_names;
    int m_suggest_index = -1;
};

#endif
