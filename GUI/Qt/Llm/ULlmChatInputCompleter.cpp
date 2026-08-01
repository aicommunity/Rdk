#include "ULlmChatInputCompleter.h"

#include "../../../LLM/Core/Gui/ULLMChatNameCompletion.h"
#include "../../../LLM/Core/LlmPublicApi.h"
#include "../../../LLM/Core/Domain/URdkDomainAccess.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QPlainTextEdit>
#include <QStringListModel>
#include <QTextCursor>

ULlmChatInputCompleter::ULlmChatInputCompleter(QPlainTextEdit* input, RDK::UApplication* app,
                                               QObject* parent)
    : QObject(parent)
    , m_input(input)
    , m_app(app)
{
    m_completer = new QCompleter(this);
    m_completer->setWidget(m_input);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchStartsWith);
    m_completer->setModel(new QStringListModel(this));
    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated), this,
            &ULlmChatInputCompleter::applyCompletion);
    refreshDictionary();
}

void ULlmChatInputCompleter::setApplication(RDK::UApplication* app)
{
    m_app = app;
    refreshDictionary();
}

void ULlmChatInputCompleter::refreshDictionary(int channel_index)
{
    m_long_names.clear();
    m_class_names.clear();
    m_property_names.clear();
    m_suggest_index = -1;

    if(!RDK::LLM::LLMServices::instance().isInitialized())
        return;

    RDK::LLM::URdkDomainAccess& domain = RDK::LLM::LLMServices::instance().domain();
    nlohmann::json snap;
    if(domain.listNetSnapshot(snap, channel_index, 5000).ok())
    {
        for(const auto& c : snap.value("components", nlohmann::json::array()))
        {
            if(!c.is_object())
                continue;
            const std::string ln = c.value("long_name", "");
            const std::string sn = c.value("short_name", "");
            if(!ln.empty())
                m_long_names.push_back(QString::fromStdString(ln));
            if(!sn.empty())
                m_long_names.push_back(QString::fromStdString(sn));
        }
        m_long_names.removeDuplicates();
    }

    nlohmann::json classes;
    if(domain.listRegisteredClasses(classes).ok())
    {
        // listRegisteredClasses returns array or object with classes — accept both.
        const nlohmann::json* arr = nullptr;
        if(classes.is_array())
            arr = &classes;
        else if(classes.contains("classes") && classes["classes"].is_array())
            arr = &classes["classes"];
        if(arr)
        {
            for(const auto& item : *arr)
            {
                if(item.is_string())
                    m_class_names.push_back(QString::fromStdString(item.get<std::string>()));
                else if(item.is_object() && item.contains("name") && item["name"].is_string())
                    m_class_names.push_back(
                        QString::fromStdString(item["name"].get<std::string>()));
                else if(item.is_object() && item.contains("class_name")
                        && item["class_name"].is_string())
                    m_class_names.push_back(
                        QString::fromStdString(item["class_name"].get<std::string>()));
            }
        }
        m_class_names.removeDuplicates();
    }

    // Property cache: focused component if any in long_names top-level + common Output/Input.
    m_property_names = QStringList{QStringLiteral("Output"), QStringLiteral("Input")};
    constexpr int kMaxPropComps = 24;
    int n = 0;
    for(const QString& ln : m_long_names)
    {
        if(ln.contains(QLatin1Char('.')))
            continue; // prefer roots first for property dump
        nlohmann::json props;
        if(!domain.getComponentProperties(ln.toStdString(), props, channel_index).ok())
            continue;
        for(const auto& p : props.value("properties", nlohmann::json::array()))
        {
            if(p.is_object() && p.contains("name") && p["name"].is_string())
                m_property_names.push_back(QString::fromStdString(p["name"].get<std::string>()));
        }
        if(++n >= kMaxPropComps)
            break;
    }
    m_property_names.removeDuplicates();
}

QStringList ULlmChatInputCompleter::currentSuggestions() const
{
    if(!m_input)
        return {};
    const QTextCursor cur = m_input->textCursor();
    const QString all = m_input->toPlainText();
    const std::string utf8 = all.toUtf8().toStdString();
    // Approximate cursor byte offset via UTF-16→UTF-8 of prefix.
    const QString prefix = all.left(cur.position());
    const size_t byte_pos = static_cast<size_t>(prefix.toUtf8().size());

    const RDK::LLM::ChatNameTokenSpan span =
        RDK::LLM::extractChatNameToken(utf8, byte_pos);

    std::vector<std::string> long_v;
    std::vector<std::string> class_v;
    std::vector<std::string> prop_v;
    long_v.reserve(static_cast<size_t>(m_long_names.size()));
    for(const QString& s : m_long_names)
        long_v.push_back(s.toStdString());
    for(const QString& s : m_class_names)
        class_v.push_back(s.toStdString());
    for(const QString& s : m_property_names)
        prop_v.push_back(s.toStdString());

    const std::vector<std::string> ranked = RDK::LLM::filterCompletionsForScopedToken(
        span.text, long_v, class_v, prop_v, 32);

    QStringList out;
    for(const std::string& s : ranked)
        out.push_back(QString::fromStdString(s));
    return out;
}

void ULlmChatInputCompleter::applyCompletion(const QString& completion)
{
    if(!m_input || completion.isEmpty())
        return;
    QTextCursor cur = m_input->textCursor();
    const QString all = m_input->toPlainText();
    const QString prefix = all.left(cur.position());
    const size_t byte_pos = static_cast<size_t>(prefix.toUtf8().size());
    const std::string utf8 = all.toUtf8().toStdString();
    const RDK::LLM::ChatNameTokenSpan span =
        RDK::LLM::extractChatNameToken(utf8, byte_pos);

    // Map byte span back to QString positions via UTF-8 prefix lengths.
    const int q_begin =
        QString::fromUtf8(utf8.data(), static_cast<int>(span.begin)).size();
    const int q_end =
        QString::fromUtf8(utf8.data(), static_cast<int>(span.end)).size();

    QString insert = completion;
            const int colon_in_token = QString::fromStdString(span.text).indexOf(QLatin1Char(':'));
    if(colon_in_token >= 0 && !completion.contains(QLatin1Char(':')))
    {
        const QString left = QString::fromStdString(span.text).left(colon_in_token + 1);
        insert = left + completion;
    }

    cur.setPosition(q_begin);
    cur.setPosition(q_end, QTextCursor::KeepAnchor);
    cur.insertText(insert);
    m_input->setTextCursor(cur);
}

bool ULlmChatInputCompleter::handleTab(bool forward)
{
    if(!m_input)
        return false;

    const QStringList suggestions = currentSuggestions();
    if(suggestions.isEmpty())
        return false;

    if(m_completer && m_completer->popup() && m_completer->popup()->isVisible())
    {
        // Cycle within popup
        if(forward)
            m_suggest_index = (m_suggest_index + 1) % suggestions.size();
        else
            m_suggest_index =
                (m_suggest_index - 1 + suggestions.size()) % suggestions.size();
    }
    else
    {
        m_suggest_index = forward ? 0 : suggestions.size() - 1;
        if(auto* model = qobject_cast<QStringListModel*>(m_completer->model()))
            model->setStringList(suggestions);
        m_completer->setCompletionPrefix(QString());
        const QRect cr = m_input->cursorRect();
        m_completer->complete(cr);
    }

    if(m_suggest_index >= 0 && m_suggest_index < suggestions.size())
    {
        applyCompletion(suggestions.at(m_suggest_index));
        // Keep popup in sync
        if(auto* model = qobject_cast<QStringListModel*>(m_completer->model()))
            model->setStringList(suggestions);
        m_completer->setCurrentRow(m_suggest_index);
    }
    return true;
}
