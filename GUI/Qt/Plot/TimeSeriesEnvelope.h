#ifndef TIME_SERIES_ENVELOPE_H
#define TIME_SERIES_ENVELOPE_H

#include <QPointF>
#include <QVector>
#include <algorithm>
#include <cmath>
#include <list>
#include <limits>

namespace NMSDK::Plot
{
inline QVector<QPointF> timeSeriesEnvelope(
    const std::list<double>& times, const std::list<double>& values, double offset,
    double viewMin = -std::numeric_limits<double>::infinity(),
    double viewMax = std::numeric_limits<double>::infinity())
{
    const auto count = std::min(times.size(), values.size());
    auto x = times.begin();
    auto y = values.begin();
    std::size_t first = 0;
    while (first < count && *x < viewMin)
    {
        ++first;
        ++x;
        ++y;
    }
    if (first > 0)
    {
        --first;
        --x;
        --y;
    }
    auto end = x;
    auto last = first;
    while (last < count && *end <= viewMax)
    {
        ++last;
        ++end;
    }
    last = std::min(count, last + 1); // Keep the sample adjacent to each edge.
    const auto visible = last - first;
    constexpr std::size_t budget = 8000;
    QVector<QPointF> points;
    points.reserve(static_cast<int>(std::min(visible, budget)));
    const auto next = [&]() {
        const QPointF point(*x, *y + offset);
        ++x;
        ++y;
        return point;
    };
    if (visible <= budget)
    {
        for (std::size_t i = 0; i < visible; ++i)
            points.push_back(next());
        return points;
    }

    points.push_back(next());
    constexpr std::size_t maxBuckets = (budget - 2) / 2;
    const auto bucketSize = (visible - 2 + maxBuckets - 1) / maxBuckets;
    for (std::size_t i = 1; i < visible - 1;)
    {
        const auto bucketEnd = std::min(i + bucketSize, visible - 1);
        auto minimumIndex = i;
        auto maximumIndex = i;
        QPointF minimum = next();
        QPointF maximum = minimum;
        for (++i; i < bucketEnd; ++i)
        {
            const QPointF point = next();
            if (std::isfinite(point.y()) && (!std::isfinite(minimum.y()) || point.y() < minimum.y()))
            {
                minimum = point;
                minimumIndex = i;
            }
            if (std::isfinite(point.y()) && (!std::isfinite(maximum.y()) || point.y() > maximum.y()))
            {
                maximum = point;
                maximumIndex = i;
            }
        }
        points.push_back(minimumIndex <= maximumIndex ? minimum : maximum);
        if (minimumIndex != maximumIndex)
            points.push_back(minimumIndex < maximumIndex ? maximum : minimum);
    }
    points.push_back(next());
    return points;
}
}

#endif
