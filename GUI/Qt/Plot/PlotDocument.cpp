#include "PlotDocument.h"

#include "../../Core/Math/UWatchablePropertyTypes.h"
#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/Utilities/USupport.h"

#include <string>

namespace NMSDK
{
namespace Plot
{

bool isWatchableLanguageType(const std::type_info& ti)
{
    return RDK::isWatchableLanguageType(ti);
}

bool isScalarWatchableLanguageType(const std::type_info& ti)
{
    return RDK::isScalarWatchableLanguageType(ti);
}

QString vizKindToString(VizKind kind)
{
    switch (kind)
    {
    case VizKind::XYLine:
        return QStringLiteral("XYLine");
    case VizKind::XYScatter:
        return QStringLiteral("XYScatter");
    case VizKind::TimeSeries:
    default:
        return QStringLiteral("TimeSeries");
    }
}

VizKind vizKindFromString(const QString& s, VizKind fallback)
{
    if (s.compare(QStringLiteral("XYLine"), Qt::CaseInsensitive) == 0)
        return VizKind::XYLine;
    if (s.compare(QStringLiteral("XYScatter"), Qt::CaseInsensitive) == 0)
        return VizKind::XYScatter;
    if (s.compare(QStringLiteral("TimeSeries"), Qt::CaseInsensitive) == 0)
        return VizKind::TimeSeries;
    return fallback;
}

static void writeRole(RDK::USerStorageXML& xml, const char* prefix, const DataRole& role)
{
    const std::string p(prefix);
    xml.WriteInteger(p + "Kind", static_cast<int>(role.kind));
    xml.WriteString(p + "Component", role.prop.component.toStdString());
    xml.WriteString(p + "Property", role.prop.property.toStdString());
    xml.WriteInteger(p + "Jx", role.prop.jx);
    xml.WriteInteger(p + "Jy", role.prop.jy);
}

static DataRole readRole(RDK::USerStorageXML& xml, const char* prefix, DataRoleKind defaultKind)
{
    DataRole role;
    const std::string p(prefix);
    role.kind = static_cast<DataRoleKind>(xml.ReadInteger(p + "Kind", static_cast<int>(defaultKind)));
    role.prop.component = QString::fromStdString(xml.ReadString(p + "Component", ""));
    role.prop.property = QString::fromStdString(xml.ReadString(p + "Property", ""));
    role.prop.jx = xml.ReadInteger(p + "Jx", -1);
    role.prop.jy = xml.ReadInteger(p + "Jy", -1);
    return role;
}

void savePlotDocument(RDK::USerStorageXML& xml, const PlotDocument& doc)
{
    xml.WriteInteger("schemaVersion", doc.schemaVersion);
    xml.WriteInteger("GridColCount", doc.gridCols);
    xml.WriteInteger("GridRowCount", doc.gridRows);
    xml.WriteInteger("GraphCount", doc.panels.size());

    xml.WriteInteger("ColSplitterCount", doc.colSplitterSizes.size());
    for (int i = 0; i < doc.colSplitterSizes.size(); ++i)
        xml.WriteInteger("ColSplitter_" + RDK::sntoa(i), doc.colSplitterSizes[i]);

    xml.WriteInteger("RowSplitterRowCount", doc.rowSplitterSizes.size());
    for (int r = 0; r < doc.rowSplitterSizes.size(); ++r)
    {
        const QList<int>& sizes = doc.rowSplitterSizes[r];
        xml.WriteInteger("RowSplitterCount_" + RDK::sntoa(r), sizes.size());
        for (int c = 0; c < sizes.size(); ++c)
            xml.WriteInteger("RowSplitter_" + RDK::sntoa(r) + "_" + RDK::sntoa(c), sizes[c]);
    }

    for (int graphIndex = 0; graphIndex < doc.panels.size(); ++graphIndex)
    {
        const PlotPanel& panel = doc.panels[graphIndex];
        xml.SelectNodeForce("graph_" + RDK::sntoa(graphIndex));

        xml.WriteString("VizKind", vizKindToString(panel.viz).toStdString());
        xml.WriteString("ChartTitle", panel.title.toStdString());
        xml.WriteString("AxisXName", panel.axisXName.toStdString());
        xml.WriteString("AxisYName", panel.axisYName.toStdString());
        xml.WriteFloat("AxisYmin", panel.axisYMin);
        xml.WriteFloat("AxisYmax", panel.axisYMax);
        xml.WriteFloat("AxisXrange", panel.axisXRange);
        xml.WriteInteger("LegendVisible", panel.legendVisible ? 1 : 0);
        xml.WriteInteger("TitleVisible", panel.titleVisible ? 1 : 0);
        xml.WriteInteger("TrackLatest", panel.trackLatest ? 1 : 0);
        xml.WriteInteger("InteractionMode", static_cast<int>(panel.interaction));
        xml.WriteInteger("UpdateIntervalMs", panel.updateIntervalMs);
        xml.WriteInteger("SeriesCount", panel.series.size());

        for (int serieIndex = 0; serieIndex < panel.series.size(); ++serieIndex)
        {
            const PlotSeries& serie = panel.series[serieIndex];
            xml.SelectNodeForce("serie_" + RDK::sntoa(serieIndex));

            xml.WriteString("SerieName", serie.visual.displayName.toStdString());
            xml.WriteInteger("SerieWidth", serie.visual.width);
            xml.WriteInteger("SerieLineType", serie.visual.penStyle);
            xml.WriteInteger("SerieColor", static_cast<int>(serie.visual.color.rgb()));
            xml.WriteFloat("SerieYShift", serie.yOffset);
            xml.WriteInteger("SerieEnabled", serie.enabled ? 1 : 0);
            xml.WriteInteger("SerieChannel", serie.binding.channel);
            xml.WriteInteger("SerieWindowSize", serie.binding.windowSize);

            xml.WriteString("SerieNameComponent", serie.binding.y.prop.component.toStdString());
            xml.WriteString("SerieNameProperty", serie.binding.y.prop.property.toStdString());
            xml.WriteInteger("SerieJx", serie.binding.y.prop.jx);
            xml.WriteInteger("SerieJy", serie.binding.y.prop.jy);

            writeRole(xml, "SerieX", serie.binding.x);
            writeRole(xml, "SerieY", serie.binding.y);

            xml.SelectUp();
        }
        xml.SelectUp();
    }
}

bool loadPlotDocument(RDK::USerStorageXML& xml, PlotDocument& doc)
{
    doc = PlotDocument();
    doc.schemaVersion = xml.ReadInteger("schemaVersion", 1);
    doc.gridCols = xml.ReadInteger("GridColCount", 1);
    doc.gridRows = xml.ReadInteger("GridRowCount", 1);

    const int colSplitCount = xml.ReadInteger("ColSplitterCount", 0);
    for (int i = 0; i < colSplitCount; ++i)
        doc.colSplitterSizes.push_back(xml.ReadInteger("ColSplitter_" + RDK::sntoa(i), 0));
    const int rowSplitRows = xml.ReadInteger("RowSplitterRowCount", 0);
    doc.rowSplitterSizes.resize(rowSplitRows);
    for (int r = 0; r < rowSplitRows; ++r)
    {
        const int n = xml.ReadInteger("RowSplitterCount_" + RDK::sntoa(r), 0);
        for (int c = 0; c < n; ++c)
            doc.rowSplitterSizes[r].push_back(
                xml.ReadInteger("RowSplitter_" + RDK::sntoa(r) + "_" + RDK::sntoa(c), 0));
    }

    const int graphCount = xml.ReadInteger("GraphCount", 0);
    if (graphCount <= 0)
        return false;

    doc.panels.reserve(graphCount);
    for (int graphIndex = 0; graphIndex < graphCount; ++graphIndex)
    {
        xml.SelectNodeForce("graph_" + RDK::sntoa(graphIndex));
        PlotPanel panel;
        panel.id = QStringLiteral("graph_%1").arg(graphIndex);
        panel.viz = vizKindFromString(
            QString::fromStdString(xml.ReadString("VizKind", "TimeSeries")),
            VizKind::TimeSeries);
        panel.title = QString::fromStdString(xml.ReadString("ChartTitle", ""));
        panel.axisXName = QString::fromStdString(xml.ReadString("AxisXName", "time, sec"));
        panel.axisYName = QString::fromStdString(xml.ReadString("AxisYName", "Output parameter"));
        panel.axisYMin = xml.ReadFloat("AxisYmin", -1.0);
        panel.axisYMax = xml.ReadFloat("AxisYmax", 1.0);
        panel.axisXRange = xml.ReadFloat("AxisXrange", 5.0);
        panel.legendVisible = xml.ReadInteger("LegendVisible", 1) != 0;
        panel.titleVisible = xml.ReadInteger("TitleVisible", 1) != 0;
        panel.trackLatest = xml.ReadInteger("TrackLatest", 1) != 0;
        panel.updateIntervalMs = xml.ReadInteger("UpdateIntervalMs", 200);
        const int interaction = xml.ReadInteger(
            "InteractionMode",
            panel.trackLatest ? static_cast<int>(InteractionMode::TrackLatest)
                              : static_cast<int>(InteractionMode::FrozenZoom));
        panel.interaction = static_cast<InteractionMode>(interaction);
        if (panel.interaction == InteractionMode::TrackLatest)
            panel.trackLatest = true;
        else if (panel.interaction == InteractionMode::FrozenZoom)
            panel.trackLatest = false;

        const int seriesCount = xml.ReadInteger("SeriesCount", 0);
        for (int serieIndex = 0; serieIndex < seriesCount; ++serieIndex)
        {
            xml.SelectNodeForce("serie_" + RDK::sntoa(serieIndex));
            PlotSeries serie;
            serie.id = QStringLiteral("serie_%1_%2").arg(graphIndex).arg(serieIndex);
            serie.visual.displayName =
                QString::fromStdString(xml.ReadString("SerieName", ""));
            serie.visual.width = xml.ReadInteger("SerieWidth", 2);
            serie.visual.penStyle = xml.ReadInteger("SerieLineType", 1);
            serie.visual.color = QColor::fromRgb(
                static_cast<QRgb>(xml.ReadInteger("SerieColor", static_cast<int>(QColor(Qt::blue).rgb()))));
            serie.yOffset = xml.ReadFloat("SerieYShift", 0.0);
            serie.enabled = xml.ReadInteger("SerieEnabled", 1) != 0;

            const int channel = xml.ReadInteger("SerieChannel", 0);
            serie.binding.channel = channel;
            serie.binding.windowSize = xml.ReadInteger("SerieWindowSize", 10000);

            // Role fields (SerieYKind/…) — источник истины, даже если schemaVersion
            // в файле отсутствует или равен 1 (частично записанный v2).
            if (xml.ReadInteger("SerieYKind", -1) >= 0)
            {
                serie.binding.x = readRole(xml, "SerieX", DataRoleKind::Time);
                serie.binding.y = readRole(xml, "SerieY", DataRoleKind::Property);
                serie.binding.channel = channel;
            }
            else
            {
                // v1: Y property only, X = Time
                const QString nameComp =
                    QString::fromStdString(xml.ReadString("SerieNameComponent", ""));
                const QString nameProp =
                    QString::fromStdString(xml.ReadString("SerieNameProperty", ""));
                const int jx = xml.ReadInteger("SerieJx", -1);
                const int jy = xml.ReadInteger("SerieJy", -1);
                serie.binding = makeTimeSeriesBinding(channel, nameComp, nameProp, jx, jy);

                // Optional v2-ish X role without full schema bump
                const int xKind = xml.ReadInteger("SerieXKind", -1);
                if (xKind == static_cast<int>(DataRoleKind::Property))
                {
                    serie.binding.x = readRole(xml, "SerieX", DataRoleKind::Property);
                    if (panel.viz == VizKind::TimeSeries)
                        panel.viz = VizKind::XYLine;
                }
            }

            if (serie.visual.displayName.isEmpty()
                && !serie.binding.y.prop.component.isEmpty())
            {
                serie.visual.displayName =
                    serie.binding.y.prop.component + ": " + serie.binding.y.prop.property
                    + QStringLiteral("(%1, %2)")
                          .arg(serie.binding.y.prop.jx)
                          .arg(serie.binding.y.prop.jy);
            }

            panel.series.push_back(serie);
            xml.SelectUp();
        }

        doc.panels.push_back(panel);
        xml.SelectUp();
    }

    doc.schemaVersion = PlotDocument::CurrentSchemaVersion;
    return true;
}

} // namespace Plot
} // namespace NMSDK
