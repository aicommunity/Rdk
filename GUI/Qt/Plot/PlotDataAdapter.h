#ifndef PLOT_DATA_ADAPTER_H
#define PLOT_DATA_ADAPTER_H

#include "PlotDocument.h"

#include <QPointF>
#include <QVector>
#include <QtGlobal>

namespace RDK
{
struct UControllerDataReader;
class UEnvironment;
}

namespace NMSDK
{
namespace Plot
{

bool latestReaderValue(RDK::UControllerDataReader* reader, double& outValue);
RDK::UControllerDataReader* getPropertyReader(RDK::UEnvironment* env, const PropertyRef& prop);

QVector<QPointF> sampleTimeSeries(RDK::UEnvironment* env,
                                  const PlotSeries& series,
                                  double yOffset);

QVector<QPointF> samplePropertyPair(RDK::UEnvironment* env,
                                    const PlotSeries& series,
                                    double yOffset,
                                    QVector<QPointF>& ring,
                                    double& lastXSimTime,
                                    double& lastYSimTime,
                                    double& lastAcceptSimTime);

/// Snapshot Y(x) from matrix/vector row or column (replaces curve each call).
QVector<QPointF> sampleMatrixSlicePair(const PlotSeries& series, double yOffset);

class IPlotDataAdapter
{
public:
    virtual ~IPlotDataAdapter() = default;
    virtual QVector<QPointF> sample(RDK::UEnvironment* env,
                                    const PlotSeries& series,
                                    double yOffset,
                                    QVector<QPointF>* xyRing,
                                    double* lastXSimTime,
                                    double* lastYSimTime,
                                    double* lastAcceptSimTime) = 0;
};

class TimeSeriesAdapter : public IPlotDataAdapter
{
public:
    QVector<QPointF> sample(RDK::UEnvironment* env,
                            const PlotSeries& series,
                            double yOffset,
                            QVector<QPointF>* xyRing,
                            double* lastXSimTime,
                            double* lastYSimTime,
                            double* lastAcceptSimTime) override
    {
        Q_UNUSED(xyRing);
        Q_UNUSED(lastXSimTime);
        Q_UNUSED(lastYSimTime);
        Q_UNUSED(lastAcceptSimTime);
        return sampleTimeSeries(env, series, yOffset);
    }
};

class PropertyPairAdapter : public IPlotDataAdapter
{
public:
    QVector<QPointF> sample(RDK::UEnvironment* env,
                            const PlotSeries& series,
                            double yOffset,
                            QVector<QPointF>* xyRing,
                            double* lastXSimTime,
                            double* lastYSimTime,
                            double* lastAcceptSimTime) override
    {
        if (!xyRing || !lastXSimTime || !lastYSimTime || !lastAcceptSimTime)
            return {};
        return samplePropertyPair(env, series, yOffset, *xyRing, *lastXSimTime, *lastYSimTime,
                                  *lastAcceptSimTime);
    }
};

inline IPlotDataAdapter* adapterFor(VizKind viz)
{
    static TimeSeriesAdapter timeSeries;
    static PropertyPairAdapter propertyPair;
    switch (viz)
    {
    case VizKind::XYLine:
    case VizKind::XYScatter:
        return &propertyPair;
    case VizKind::TimeSeries:
    default:
        return &timeSeries;
    }
}

} // namespace Plot
} // namespace NMSDK

#endif // PLOT_DATA_ADAPTER_H
