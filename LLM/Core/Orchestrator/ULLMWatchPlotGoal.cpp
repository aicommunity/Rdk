#include "ULLMWatchPlotGoal.h"

#include "../Context/ULLMConnectSemanticsCatalog.h"

#include <QString>
#include <QRegularExpression>

#include <algorithm>
#include <cctype>

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

bool equalsCi(const std::string& a, const std::string& b)
{
    if(a.size() != b.size())
        return false;
    for(size_t i = 0; i < a.size(); ++i)
    {
        const char ca = static_cast<char>(std::tolower(static_cast<unsigned char>(a[i])));
        const char cb = static_cast<char>(std::tolower(static_cast<unsigned char>(b[i])));
        if(ca != cb)
            return false;
    }
    return true;
}

std::string detectNestedHint(const std::string& lower)
{
    std::string from_port;
    std::string to_port;
    if(defaultConnectSemanticsCatalog().matchGoalPortHint(lower, from_port, to_port)
       && !from_port.empty())
        return from_port;

    // Soft / partial: LTZone token without requiring a full connect pair match.
    if(contains(lower, "ltzone") || contains(lower, "lt zone")
       || contains(lower, "низкопорог"))
        return "LTZone";

    // Labels may set from_port even when matchGoalPortHint returns false (no to_port).
    for(const char* label : {"низкопороговая зона", "low-threshold zone", "low threshold zone",
                             "lt zone"})
    {
        if(contains(lower, label))
            return "LTZone";
    }
    return {};
}

} // namespace

bool isWatchPlotGoal(const std::string& text)
{
    const std::string lower = toLowerUtf8(text);
    if(lower.empty())
        return false;
    // «график» / typo «графки» / «на граф…»
    return contains(lower, "график") || contains(lower, "графк") || contains(lower, "watch")
           || contains(lower, "plot") || contains(lower, "watches");
}

std::string watchPlotEphemeralHint()
{
    return "Plotting signals: use add_watch_series(long_name, property_name) on surface=window "
           "(standalone Watch via show_ui_panel panel=watch) or surface=mdi after "
           "create_watch_mdi. Nested signals use dotted long_name Parent.Child "
           "(e.g. PNeuron.LTZone) with property_name on the child — not parent.Output alone. "
           "If ComponentNotFound, call find_component / get_net_snapshot; do not use "
           "search_project_docs for live model names. Do not use open_component_gui_tab for "
           "chart series. list_watch_mdi / focus_watch_mdi / close_watch_mdi manage MDI "
           "Watches_N windows.";
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

    out.nested_hint = detectNestedHint(lower);

    const QString q = QString::fromUtf8(text.data(), static_cast<int>(text.size()));
    // Tokens like PGenerator, PNeuron, LTZone, Generator
    QRegularExpression tokRe(QStringLiteral("\\b(P[A-Za-z][A-Za-z0-9_]*|[A-Z][A-Za-z0-9_]{2,})\\b"));
    QRegularExpressionMatchIterator it = tokRe.globalMatch(q);
    while(it.hasNext())
    {
        const QString t = it.next().captured(1);
        const QString tl = t.toLower();
        if(tl == QLatin1String("output") || tl == QLatin1String("input") || tl == QLatin1String("watch")
           || tl == QLatin1String("watches") || tl == QLatin1String("plot"))
            continue;
        const std::string tok = t.toUtf8().toStdString();
        if(!out.nested_hint.empty() && equalsCi(tok, out.nested_hint))
            continue; // nested role is not a root component token
        pushUniqueToken(out.component_tokens, tok);
    }

    // Soft lowercase instance tokens (e.g. "ltzone pneuron")
    QRegularExpression softTokRe(
        QStringLiteral("\\b(pneuron|pgenerator|generator|neuron)\\b"),
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator sit = softTokRe.globalMatch(q);
    while(sit.hasNext())
    {
        const QString tl = sit.next().captured(1).toLower();
        if(tl == QLatin1String("pneuron") || tl == QLatin1String("neuron"))
            pushUniqueToken(out.component_tokens, "PNeuron");
        else if(tl == QLatin1String("pgenerator") || tl == QLatin1String("generator"))
            pushUniqueToken(out.component_tokens, "PGenerator");
    }

    // Soft RU/EN class hints — merge even when CapWords present (e.g. "Output генератора").
    if(contains(lower, "генератор") || contains(lower, "generator"))
        pushUniqueToken(out.component_tokens, "PGenerator");
    if(contains(lower, "нейрон") || contains(lower, "neuron") || contains(lower, "pneuron"))
        pushUniqueToken(out.component_tokens, "PNeuron");

    if(contains(lower, "выход") || contains(lower, "output"))
        out.property_name = "Output";

    if(!out.nested_hint.empty())
        out.anchor_tokens = out.component_tokens;

    // ok when we have components to plot, nested+anchor, or an explicit new-MDI request.
    out.ok = !out.component_tokens.empty() || out.want_new_mdi
             || (!out.nested_hint.empty() && !out.anchor_tokens.empty());
    return out;
}

} // namespace RDK::LLM
