#include "PlotDataAdapter.h"

#include "../../Core/Engine/UController.h"
#include "../../Core/Engine/UEnvironment.h"

#include <list>

namespace NMSDK
{
namespace Plot
{

RDK::UControllerDataReader* getPropertyReader(RDK::UEnvironment* env, const PropertyRef& prop)
{
    if (!env || prop.component.isEmpty() || prop.property.isEmpty())
        return nullptr;
    return env->GetDataReader(prop.component.toStdString(),
                              prop.property.toStdString(),
                              prop.jx < 0 ? 0 : prop.jx,
                              prop.jy < 0 ? 0 : prop.jy);
}

bool latestReaderValue(RDK::UControllerDataReader* reader, double& outValue)
{
    if (!reader || reader->YData.empty())
        return false;
    outValue = reader->YData.back();
    return true;
}

QVector<QPointF> sampleTimeSeries(RDK::UEnvironment* env,
                                  const PlotSeries& series,
                                  double yOffset)
{
    QVector<QPointF> points;
    RDK::UControllerDataReader* reader = getPropertyReader(env, series.binding.y.prop);
    if (!reader)
        return points;

    std::list<double> XData = reader->XData;
    std::list<double> YData = reader->YData;

    const int maxPoints = series.binding.windowSize > 0 ? series.binding.windowSize : 10000;
    if (static_cast<int>(XData.size()) > maxPoints)
    {
        auto xIt = XData.begin();
        auto yIt = YData.begin();
        std::advance(xIt, static_cast<int>(XData.size()) - maxPoints);
        std::advance(yIt, static_cast<int>(YData.size()) - maxPoints);
        XData.erase(XData.begin(), xIt);
        YData.erase(YData.begin(), yIt);
    }

    points.reserve(static_cast<int>(XData.size()));
    for (auto itx = XData.begin(), ity = YData.begin();
         itx != XData.end() && ity != YData.end();
         ++itx, ++ity)
    {
        points.push_back(QPointF(*itx, *ity + yOffset));
    }
    return points;
}

QVector<QPointF> samplePropertyPair(RDK::UEnvironment* env,
                                    const PlotSeries& series,
                                    double yOffset,
                                    QVector<QPointF>& ring,
                                    int& lastXCount,
                                    int& lastYCount)
{
    RDK::UControllerDataReader* xReader = getPropertyReader(env, series.binding.x.prop);
    RDK::UControllerDataReader* yReader = getPropertyReader(env, series.binding.y.prop);
    if (!xReader || !yReader)
        return ring;

    const int xCount = static_cast<int>(xReader->YData.size());
    const int yCount = static_cast<int>(yReader->YData.size());
    if (xCount > 0 && yCount > 0 && (xCount != lastXCount || yCount != lastYCount))
    {
        double xv = 0.0;
        double yv = 0.0;
        if (latestReaderValue(xReader, xv) && latestReaderValue(yReader, yv))
        {
            ring.push_back(QPointF(xv, yv + yOffset));
            const int maxPoints =
                series.binding.windowSize > 0 ? series.binding.windowSize : 10000;
            while (ring.size() > maxPoints)
                ring.remove(0);
        }
        lastXCount = xCount;
        lastYCount = yCount;
    }
    return ring;
}

} // namespace Plot
} // namespace NMSDK
