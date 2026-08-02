#ifndef PLOT_DOCUMENT_H
#define PLOT_DOCUMENT_H

#include <QColor>
#include <QList>
#include <QString>
#include <QVector>
#include <string>
#include <typeinfo>

namespace RDK
{
class USerStorageXML;
}

namespace NMSDK
{
namespace Plot
{

enum class DataRoleKind
{
    Time = 0,
    Property = 1
};

enum class VizKind
{
    TimeSeries = 0,
    XYLine = 1,
    XYScatter = 2
};

enum class InteractionMode
{
    TrackLatest = 0,
    FrozenZoom = 1,
    Pan = 2
};

struct PropertyRef
{
    QString component;
    QString property;
    int jx = -1;
    int jy = -1;
};

struct DataRole
{
    DataRoleKind kind = DataRoleKind::Time;
    PropertyRef prop;
};

struct DataBinding
{
    int channel = 0;
    DataRole x;
    DataRole y;
    int windowSize = 10000;
};

struct PlotSeriesVisual
{
    QString displayName;
    QColor color = Qt::blue;
    int width = 2;
    int penStyle = 1; // Qt::SolidLine
};

struct PlotSeries
{
    QString id;
    PlotSeriesVisual visual;
    double yOffset = 0.0;
    bool enabled = true;
    DataBinding binding;
};

struct PlotPanel
{
    QString id;
    VizKind viz = VizKind::TimeSeries;
    QString title;
    QString axisXName = QStringLiteral("time, sec");
    QString axisYName = QStringLiteral("Output parameter");
    double axisYMin = -1.0;
    double axisYMax = 1.0;
    double axisXRange = 5.0;
    bool legendVisible = true;
    bool titleVisible = true;
    bool trackLatest = true;
    InteractionMode interaction = InteractionMode::TrackLatest;
    int updateIntervalMs = 200;
    QVector<PlotSeries> series;
};

struct PlotDocument
{
    static constexpr int CurrentSchemaVersion = 2;

    int schemaVersion = CurrentSchemaVersion;
    int gridRows = 1;
    int gridCols = 1;
    QList<int> colSplitterSizes;
    QVector<QList<int>> rowSplitterSizes;
    QVector<PlotPanel> panels;
};

inline DataBinding makeTimeSeriesBinding(int channel,
                                         const QString& component,
                                         const QString& property,
                                         int jx,
                                         int jy)
{
    DataBinding b;
    b.channel = channel;
    b.x.kind = DataRoleKind::Time;
    b.y.kind = DataRoleKind::Property;
    b.y.prop.component = component;
    b.y.prop.property = property;
    b.y.prop.jx = jx;
    b.y.prop.jy = jy;
    return b;
}

inline DataBinding makeXYBinding(int channel,
                                 const PropertyRef& xProp,
                                 const PropertyRef& yProp,
                                 int windowSize = 10000)
{
    DataBinding b;
    b.channel = channel;
    b.windowSize = windowSize;
    b.x.kind = DataRoleKind::Property;
    b.x.prop = xProp;
    b.y.kind = DataRoleKind::Property;
    b.y.prop = yProp;
    return b;
}

QString vizKindToString(VizKind kind);
VizKind vizKindFromString(const QString& s, VizKind fallback = VizKind::TimeSeries);

/// Watchable property types (double/int and MDMatrix/MDVector of those).
bool isWatchableLanguageType(const std::type_info& ti);
bool isScalarWatchableLanguageType(const std::type_info& ti);

/// Save PlotDocument layout into current XML node (UWatchTab node).
void savePlotDocument(RDK::USerStorageXML& xml, const PlotDocument& doc);

/// Load PlotDocument from current XML node. Supports schema v1 (no version) and v2+.
bool loadPlotDocument(RDK::USerStorageXML& xml, PlotDocument& doc);

} // namespace Plot
} // namespace NMSDK

#endif // PLOT_DOCUMENT_H
