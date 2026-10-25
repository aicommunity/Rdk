#include "Plot/PlotDataAdapter.h"

#include <gtest/gtest.h>

TEST(PlotDecimation, EnvelopePreservesPeak)
{
    QVector<QPointF> src;
    src.reserve(1000);
    for (int i = 0; i < 1000; ++i)
        src.push_back(QPointF(static_cast<double>(i), i == 500 ? 100.0 : 0.0));

    const QVector<QPointF> out = NMSDK::Plot::decimatePointsEnvelope(src, 80);
    double maxY = -1e9;
    for (const QPointF& p : out)
        maxY = qMax(maxY, p.y());
    EXPECT_GE(maxY, 99.0);
}
