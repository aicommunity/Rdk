#include "UWatchSerie.h"

#include <gtest/gtest.h>

TEST(UWatchSerieIdentity, CapturesKeepGeneratedIdentity)
{
    UWatchSerie serie;

    const QString id = serie.toPlotSeries().id;
    ASSERT_FALSE(id.isEmpty());
    EXPECT_EQ(id, serie.toPlotSeries().id);
    EXPECT_EQ(id, serie.toPlotSeries().id);
}

TEST(UWatchSerieIdentity, RestoredIdentitySurvivesCapture)
{
    UWatchSerie serie;
    const QString restoredId = QStringLiteral("serie-restored-identity");
    serie.setPlotSeriesId(restoredId);

    EXPECT_EQ(restoredId, serie.toPlotSeries().id);
    EXPECT_EQ(restoredId, serie.toPlotSeries().id);
}
