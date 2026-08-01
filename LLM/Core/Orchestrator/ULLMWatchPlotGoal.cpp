#include "ULLMWatchPlotGoal.h"

#include <QString>
#include <QRegularExpression>

#include <algorithm>

namespace RDK::LLM {

namespace {

std::string toLowerUtf8(const std::string& s)
{
    QString q = QString::fromUtf8(s.data(), static_cast<int>(s.size())).toLower();
    q.replace(QString::fromUtf8("ё"), QString::fromUtf8("е"));
    return q.toUtf8().toStdString();
}

bool contains(const std::string& hay, const char* needle)
{
    return hay.find(needle) != std::string::npos;
}

void pushUniqueToken(std::vector<std::string>& tokens, const std::string& token)
{
    if(token.empty())
        return;
    if(std::find(tokens.begin(), tokens.end(), token) != tokens.end())
        return;
    tokens.push_back(token);
}

} // namespace

bool isWatchPlotGoal(const std::string& text)
{
    const std::string lower = toLowerUtf8(text);
    if(lower.empty())
        return false;
    return contains(lower, "график") || contains(lower, "watch") || contains(lower, "plot")
           || contains(lower, "на график") || contains(lower, "watches");
}

std::string watchPlotEphemeralHint()
{
    return "Plotting signals: use add_watch_series(long_name, property_name) on surface=window "
           "(standalone Watch via show_ui_panel panel=watch) or surface=mdi after "
           "create_watch_mdi. Do not use open_component_gui_tab to add chart series. "
           "list_watch_mdi / focus_watch_mdi / close_watch_mdi manage MDI Watches_N windows.";
}

ParsedWatchPlotGoal parseWatchPlotGoal(const std::string& text)
{
    ParsedWatchPlotGoal out;
    if(!isWatchPlotGoal(text))
        return out;

    const std::string lower = toLowerUtf8(text);
    out.want_new_mdi = contains(lower, "новое окно") || contains(lower, "отдельн")
                       || contains(lower, "новый watches") || contains(lower, "create watch")
                       || contains(lower, "отдельное watches") || contains(lower, "создай watches");

    const QString q = QString::fromUtf8(text.data(), static_cast<int>(text.size()));
    // Tokens like PGenerator, PNeuron, Generator
    QRegularExpression tokRe(QStringLiteral("\\b(P[A-Za-z][A-Za-z0-9_]*|[A-Z][A-Za-z0-9_]{2,})\\b"));
    QRegularExpressionMatchIterator it = tokRe.globalMatch(q);
    while(it.hasNext())
    {
        const QString t = it.next().captured(1);
        const QString tl = t.toLower();
        if(tl == QLatin1String("output") || tl == QLatin1String("input") || tl == QLatin1String("watch")
           || tl == QLatin1String("watches") || tl == QLatin1String("plot"))
            continue;
        pushUniqueToken(out.component_tokens, t.toUtf8().toStdString());
    }

    // Soft RU/EN class hints — merge even when CapWords present (e.g. "Output генератора").
    if(contains(lower, "генератор") || contains(lower, "generator"))
        pushUniqueToken(out.component_tokens, "PGenerator");
    if(contains(lower, "нейрон") || contains(lower, "neuron"))
        pushUniqueToken(out.component_tokens, "PNeuron");

    if(contains(lower, "выход") || contains(lower, "output"))
        out.property_name = "Output";

    // ok when we have components to plot, or an explicit new-MDI request (series from focus later).
    out.ok = !out.component_tokens.empty() || out.want_new_mdi;
    return out;
}

} // namespace RDK::LLM
