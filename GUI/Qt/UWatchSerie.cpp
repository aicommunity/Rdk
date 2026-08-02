#include "UWatchSerie.h"
#include <QPen>

UWatchSerie::UWatchSerie()
{
    isOnline = true;
}

void UWatchSerie::setOnlineStatus(bool online)
{
    if (isOnline == online) {
        return;
    }

    isOnline = online;
    QPen pen = this->pen();

    if (online) {
        pen.setStyle(Qt::SolidLine);
    } else {
        pen.setStyle(Qt::DashLine);
    }

    this->setPen(pen);
}

NMSDK::Plot::DataBinding UWatchSerie::toBinding() const
{
    using namespace NMSDK::Plot;
    if (vizKind == VizKind::XYLine || vizKind == VizKind::XYScatter)
    {
        PropertyRef xProp{xNameComponent, xNameProperty, xJx, xJy};
        PropertyRef yProp{nameComponent, nameProperty, Jx, Jy};
        return makeXYBinding(indexChannel, xProp, yProp, windowSize);
    }
    return makeTimeSeriesBinding(indexChannel, nameComponent, nameProperty, Jx, Jy);
}

void UWatchSerie::applyBinding(const NMSDK::Plot::DataBinding& binding, NMSDK::Plot::VizKind viz)
{
    using namespace NMSDK::Plot;
    vizKind = viz;
    indexChannel = binding.channel;
    windowSize = binding.windowSize > 0 ? binding.windowSize : 10000;

    if (binding.y.kind == DataRoleKind::Property)
    {
        nameComponent = binding.y.prop.component;
        nameProperty = binding.y.prop.property;
        Jx = binding.y.prop.jx;
        Jy = binding.y.prop.jy;
    }

    if (binding.x.kind == DataRoleKind::Property)
    {
        xNameComponent = binding.x.prop.component;
        xNameProperty = binding.x.prop.property;
        xJx = binding.x.prop.jx;
        xJy = binding.x.prop.jy;
    }
    else
    {
        xNameComponent.clear();
        xNameProperty.clear();
        xJx = -1;
        xJy = -1;
        x_data_reader = nullptr;
        xyRing.clear();
        xyLastXCount = -1;
        xyLastYCount = -1;
    }
}

NMSDK::Plot::PlotSeries UWatchSerie::toPlotSeries() const
{
    NMSDK::Plot::PlotSeries s;
    s.visual.displayName = name();
    s.visual.color = color();
    s.visual.width = pen().width();
    s.visual.penStyle = static_cast<int>(pen().style());
    s.yOffset = YShift;
    s.enabled = isOnline;
    s.binding = toBinding();
    return s;
}
