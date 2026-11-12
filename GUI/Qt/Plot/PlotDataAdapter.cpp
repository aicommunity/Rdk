#include "PlotDataAdapter.h"

#include "../../Core/Engine/UController.h"
#include "../../Core/Engine/UEnvironment.h"
#include "../../Core/Math/MDMatrix.h"
#include "../../Core/Math/MDVector.h"

#include "rdk.h"
#include <rdk_application.h>

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

    // Pass-through: reader owns history (TimeInterval / NumPoints). Do not trim by
    // windowSize or TimeInterval again — that competed with Track and clipped ends.
    const std::list<double>& XData = reader->XData;
    const std::list<double>& YData = reader->YData;
    const int n = static_cast<int>(qMin(XData.size(), YData.size()));
    if (n <= 0)
        return points;

    // Draw-budget decimation only; always keep first and last samples.
    constexpr int kDrawBudget = 8000;
    points.reserve(n > kDrawBudget ? kDrawBudget + 1 : n);
    if (n <= kDrawBudget)
    {
        auto itx = XData.begin();
        auto ity = YData.begin();
        for (int i = 0; i < n; ++i, ++itx, ++ity)
            points.push_back(QPointF(*itx, *ity + yOffset));
    }
    else
    {
        const int step = (n + kDrawBudget - 1) / kDrawBudget;
        auto itx = XData.begin();
        auto ity = YData.begin();
        for (int i = 0; i < n; ++i, ++itx, ++ity)
        {
            if (i == 0 || i == n - 1 || (i % step) == 0)
                points.push_back(QPointF(*itx, *ity + yOffset));
        }
        if (points.size() >= 2 && points[points.size() - 2] == points.last())
            points.remove(points.size() - 2);
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

namespace {

bool readSliceVector(const PropertyRef& prop, QVector<double>& out)
{
    out.clear();
    if (prop.component.isEmpty() || prop.property.isEmpty())
        return false;

    const std::string comp = prop.component.toStdString();
    const std::string name = prop.property.toStdString();

    RDK::UELockPtr<RDK::UEngine> engine = RDK::GetEngineLock();
    if (!engine)
        return false;

    // Try matrix double
    try
    {
        RDK::MDMatrix<double> m =
            engine->Model_GetComponentPropertyData<RDK::MDMatrix<double>>(comp.c_str(), name.c_str());
        const int rows = m.GetRows();
        const int cols = m.GetCols();
        if (prop.slice == SliceKind::Row)
        {
            const int r = prop.jx < 0 ? 0 : prop.jx;
            if (r < 0 || r >= rows)
                return false;
            out.reserve(cols);
            for (int c = 0; c < cols; ++c)
                out.push_back(m(r, c));
            return !out.isEmpty();
        }
        if (prop.slice == SliceKind::Column)
        {
            const int c = prop.jy < 0 ? 0 : prop.jy;
            if (c < 0 || c >= cols)
                return false;
            out.reserve(rows);
            for (int r = 0; r < rows; ++r)
                out.push_back(m(r, c));
            return !out.isEmpty();
        }
        // Cell: single value
        const int r = prop.jx < 0 ? 0 : prop.jx;
        const int c = prop.jy < 0 ? 0 : prop.jy;
        if (r >= 0 && r < rows && c >= 0 && c < cols)
        {
            out.push_back(m(r, c));
            return true;
        }
        return false;
    }
    catch (...)
    {
    }

    try
    {
        RDK::MDMatrix<int> m =
            engine->Model_GetComponentPropertyData<RDK::MDMatrix<int>>(comp.c_str(), name.c_str());
        const int rows = m.GetRows();
        const int cols = m.GetCols();
        if (prop.slice == SliceKind::Row)
        {
            const int r = prop.jx < 0 ? 0 : prop.jx;
            if (r < 0 || r >= rows)
                return false;
            out.reserve(cols);
            for (int c = 0; c < cols; ++c)
                out.push_back(static_cast<double>(m(r, c)));
            return !out.isEmpty();
        }
        if (prop.slice == SliceKind::Column)
        {
            const int c = prop.jy < 0 ? 0 : prop.jy;
            if (c < 0 || c >= cols)
                return false;
            out.reserve(rows);
            for (int r = 0; r < rows; ++r)
                out.push_back(static_cast<double>(m(r, c)));
            return !out.isEmpty();
        }
        const int r = prop.jx < 0 ? 0 : prop.jx;
        const int c = prop.jy < 0 ? 0 : prop.jy;
        if (r >= 0 && r < rows && c >= 0 && c < cols)
        {
            out.push_back(static_cast<double>(m(r, c)));
            return true;
        }
        return false;
    }
    catch (...)
    {
    }

    try
    {
        RDK::MDVector<double> v =
            engine->Model_GetComponentPropertyData<RDK::MDVector<double>>(comp.c_str(), name.c_str());
        const int n = v.GetRows() > 0 ? v.GetRows() : v.GetCols();
        out.reserve(n);
        for (int i = 0; i < n; ++i)
            out.push_back(v(i));
        return !out.isEmpty();
    }
    catch (...)
    {
    }

    try
    {
        RDK::MDVector<int> v =
            engine->Model_GetComponentPropertyData<RDK::MDVector<int>>(comp.c_str(), name.c_str());
        const int n = v.GetRows() > 0 ? v.GetRows() : v.GetCols();
        out.reserve(n);
        for (int i = 0; i < n; ++i)
            out.push_back(static_cast<double>(v(i)));
        return !out.isEmpty();
    }
    catch (...)
    {
    }

    return false;
}

} // namespace

QVector<QPointF> sampleMatrixSlicePair(const PlotSeries& series, double yOffset)
{
    QVector<double> xs;
    QVector<double> ys;
    if (!readSliceVector(series.binding.x.prop, xs) || !readSliceVector(series.binding.y.prop, ys))
        return {};

    const int n = qMin(xs.size(), ys.size());
    QVector<QPointF> points;
    points.reserve(n);
    for (int i = 0; i < n; ++i)
        points.push_back(QPointF(xs.at(i), ys.at(i) + yOffset));
    return sortedByAscendingX(points);
}

} // namespace Plot
} // namespace NMSDK
