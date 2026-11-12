#include "ULLMComponentStructureGoal.h"

#include <QString>
#include <QRegularExpression>

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

} // namespace

bool isComponentStructureGoal(const std::string& text)
{
    const std::string lower = toLowerUtf8(text);
    if(lower.empty())
        return false;
    if(contains(lower, "дендрит") || contains(lower, "dendrit"))
        return true;
    if(contains(lower, "numsoma") || contains(lower, "numdendrite")
       || contains(lower, "num_soma") || contains(lower, "num_dendrite"))
        return true;
    if(contains(lower, "structurebuildmode") || contains(lower, "structure_build"))
        return true;
    if((contains(lower, "сома") || contains(lower, "soma"))
       && (contains(lower, "membrane") || contains(lower, "сегмент") || contains(lower, "част")))
        return true;
    if(contains(lower, "membrane parts") || contains(lower, "membraneparts"))
        return true;
    if(contains(lower, "настрой") && (contains(lower, "нейрон") || contains(lower, "neuron")))
        return true;
    return false;
}

std::string componentStructureEphemeralHint()
{
    return "Pulse neuron structure (dendrites/soma) is configured by component properties, "
           "not by add_component of Dendrite classes. Canonical path: describe_class / "
           "get_component_properties, then set_property StructureBuildMode=2, "
           "NumSomaMembraneParts=<N>, NumDendriteMembranePartsVec=\"L1 L2 … LN\" "
           "(space-separated lengths; vec size must equal NumSomaMembraneParts), then "
           "calculate_component to rebuild. Do not invent a Dendrite class or call "
           "list_registered_classes looking for Dendrite.";
}

ParsedDendriteStructureGoal parseDendriteStructureGoal(const std::string& text)
{
    ParsedDendriteStructureGoal out;
    const QString q = QString::fromUtf8(text.data(), static_cast<int>(text.size()));
    const QString lower = q.toLower().replace(QString::fromUtf8("ё"), QString::fromUtf8("е"));

    // «4 дендрит» / «четыре дендрит» — prefer digit count
    QRegularExpression countRe(
        QStringLiteral("(\\d+)\\s*дендрит"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch cm = countRe.match(lower);
    if(cm.hasMatch())
        out.dendrite_count = cm.captured(1).toInt();

    // Lengths: «длинами 2, 3, 8, 1» / «длины 2 3 8 1» / «длиной: 2,3,8,1»
    QRegularExpression lenRe(
        QStringLiteral("длин[аыойамие]*\\s*[:\\s]*([0-9]+(?:\\s*[,;]\\s*[0-9]+)*)"),
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch lm = lenRe.match(lower);
    if(lm.hasMatch())
    {
        const QStringList parts =
            lm.captured(1).split(QRegularExpression(QStringLiteral("[,;\\s]+")),
                                 Qt::SkipEmptyParts);
        for(const QString& p : parts)
        {
            bool ok = false;
            const int v = p.toInt(&ok);
            if(ok && v >= 0)
                out.lengths.push_back(v);
        }
    }

    // Component token: PNeuron / PNeuronGen etc. after «у » or bare CapWord
    QRegularExpression tokRe(QStringLiteral("(?:\\bу\\s+|\\bfor\\s+)?(P[A-Za-z][A-Za-z0-9_]*)"));
    QRegularExpressionMatch tm = tokRe.match(q);
    if(tm.hasMatch())
        out.component_token = tm.captured(1).toUtf8().toStdString();
    // Soft RU/EN: «нейрон» / neuron → PNeuron when CapWord missing
    if(out.component_token.empty())
    {
        const std::string lower_s = lower.toUtf8().toStdString();
        if(contains(lower_s, "нейрон") || contains(lower_s, "neuron"))
            out.component_token = "PNeuron";
    }

    if(out.dendrite_count <= 0 && !out.lengths.empty())
        out.dendrite_count = static_cast<int>(out.lengths.size());
    if(out.dendrite_count > 0 && out.lengths.empty())
    {
        // count without lengths — not enough for FastPath
        out.ok = false;
        return out;
    }
    if(out.dendrite_count > 0 && static_cast<int>(out.lengths.size()) == out.dendrite_count
       && !out.component_token.empty())
        out.ok = true;
    else if(out.dendrite_count > 0 && static_cast<int>(out.lengths.size()) == out.dendrite_count)
    {
        // Allow FastPath later with default token from session; mark partial ok for detector
        out.ok = true;
    }
    return out;
}

} // namespace RDK::LLM
