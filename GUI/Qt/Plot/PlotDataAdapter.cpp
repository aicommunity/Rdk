#include "PlotDataAdapter.h"

#include "../../Core/Engine/UController.h"
#include "../../Core/Engine/UEnvironment.h"

#include <algorithm>
#include <cmath>
#include <list>
#include <QtGlobal>

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

static bool latestReaderSimTime(RDK::UControllerDataReader* reader, double& outTime)
{
    if (!reader || reader->XData.empty())
        return false;
    outTime = reader->XData.back();
    return true;
}

static QVector<QPointF> sortedByAscendingX(const QVector<QPointF>& ring)
{
    QVector<QPointF> out = ring;
    std::stable_sort(out.begin(), out.end(),
                     [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); });
    return out;
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
                                    double& lastXSimTime,
                                    double& lastYSimTime,
                                    double& lastAcceptSimTime)
{
    RDK::UControllerDataReader* xReader = getPropertyReader(env, series.binding.x.prop);
    RDK::UControllerDataReader* yReader = getPropertyReader(env, series.binding.y.prop);
    if (!xReader || !yReader)
        return sortedByAscendingX(ring);

    if (xReader->YData.empty() || yReader->YData.empty())
        return sortedByAscendingX(ring);

    // Gate on sim-time (XData.back), not FIFO size — size stays fixed once full.
    double xSim = 0.0;
    double ySim = 0.0;
    const bool haveXSim = latestReaderSimTime(xReader, xSim);
    const bool haveYSim = latestReaderSimTime(yReader, ySim);
    if (!haveXSim && !haveYSim)
        return sortedByAscendingX(ring);

    const bool xChanged = haveXSim && (lastXSimTime < 0.0 || xSim != lastXSimTime);
    const bool yChanged = haveYSim && (lastYSimTime < 0.0 || ySim != lastYSimTime);
    if (!xChanged && !yChanged)
        return sortedByAscendingX(ring);

    double xv = 0.0;
    double yv = 0.0;
    if (!latestReaderValue(xReader, xv) || !latestReaderValue(yReader, yv))
    {
        if (haveXSim)
            lastXSimTime = xSim;
        if (haveYSim)
            lastYSimTime = ySim;
        return sortedByAscendingX(ring);
    }

    const QPointF candidate(xv, yv + yOffset);

    double simTime = 0.0;
    bool haveSim = false;
    if (haveYSim)
    {
        simTime = ySim;
        haveSim = true;
    }
    else if (haveXSim)
    {
        simTime = xSim;
        haveSim = true;
    }

    // Same-X dwell: update existing X anywhere in the ring (Y(x) is a function of X).
    constexpr double kSameXRel = 1e-9;
    constexpr double kSameXAbs = 1e-12;
    for (int i = 0; i < ring.size(); ++i)
    {
        const QPointF& pt = ring.at(i);
        const double tol = qMax(kSameXAbs, kSameXRel * qMax(qAbs(pt.x()), qAbs(candidate.x())));
        if (qAbs(pt.x() - candidate.x()) <= tol)
        {
            ring[i] = candidate;
            if (haveSim)
                lastAcceptSimTime = simTime;
            if (haveXSim)
                lastXSimTime = xSim;
            if (haveYSim)
                lastYSimTime = ySim;
            return sortedByAscendingX(ring);
        }
    }

    bool accept = true;
    if (accept && series.binding.xyMinIntervalMs > 0 && haveSim
        && lastAcceptSimTime >= 0.0)
    {
        const double minGapSec = series.binding.xyMinIntervalMs / 1000.0;
        if ((simTime - lastAcceptSimTime) < minGapSec)
            accept = false;
    }

    // Near-duplicate: reject when close on BOTH axes to any existing point.
    if (accept && series.binding.xyMinDistance > 0.0 && !ring.isEmpty())
    {
        const double minD = series.binding.xyMinDistance;
        for (const QPointF& pt : ring)
        {
            const double dx = qAbs(pt.x() - candidate.x());
            const double dy = qAbs(pt.y() - candidate.y());
            if (dx < minD && dy < minD)
            {
                accept = false;
                break;
            }
        }
    }

    if (accept)
    {
        ring.push_back(candidate);
        if (haveSim)
            lastAcceptSimTime = simTime;
        const int maxPoints =
            series.binding.windowSize > 0 ? series.binding.windowSize : 2000;
        // Chronological FIFO: drop oldest samples (ring stays time-ordered).
        while (ring.size() > maxPoints)
            ring.remove(0);
    }

    if (haveXSim)
        lastXSimTime = xSim;
    if (haveYSim)
        lastYSimTime = ySim;
    return sortedByAscendingX(ring);
}

} // namespace Plot
} // namespace NMSDK
