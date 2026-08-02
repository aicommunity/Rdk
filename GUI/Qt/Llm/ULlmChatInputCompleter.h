#ifndef RDK_ULLM_CHAT_INPUT_COMPLETER_H
#define RDK_ULLM_CHAT_INPUT_COMPLETER_H

#include <QObject>
#include <QStringList>

class QPlainTextEdit;
class QCompleter;
class QEvent;

namespace RDK {
class UApplication;
}

/// Live + Tab name completion for LLM chat input (components / classes / properties).
class ULlmChatInputCompleter : public QObject {
    Q_OBJECT
public:
    explicit ULlmChatInputCompleter(QPlainTextEdit* input, RDK::UApplication* app,
                                    QObject* parent = nullptr);

    void setApplication(RDK::UApplication* app);
    void refreshDictionary(int channel_index = 0);
    /// Handle Tab / Shift+Tab: open or cycle highlight without inserting. Returns true if consumed.
    bool handleTab(bool forward);
    /// Hide popup on Escape. Returns true if consumed.
    bool handleEscape();
    /// If popup is visible, insert the highlighted suggestion. Returns true if consumed.
    bool acceptCurrentSuggestion();
    bool isPopupVisible() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void applyCompletion(const QString& completion);
    QStringList currentSuggestions() const;
    QString currentTokenText() const;
    bool shouldShowSuggestions(const QString& token, const QStringList& suggestions) const;
    void showSuggestionsPopup(const QStringList& suggestions);
    void hideSuggestionsPopup();
    void onInputChanged();

    QPlainTextEdit* m_input = nullptr;
    RDK::UApplication* m_app = nullptr;
    QCompleter* m_completer = nullptr;
    QStringList m_long_names;
    QStringList m_class_names;
    QStringList m_property_names;
    int m_suggest_index = -1;
    bool m_applying = false;
};

#endif
