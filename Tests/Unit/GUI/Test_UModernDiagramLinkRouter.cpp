#include "UModernDiagramLinkRouter.h"

#include <gtest/gtest.h>

#include <QList>
#include <QPointF>
#include <QRectF>

using namespace UModernDiagramLinkRouter;

namespace {

struct ChainFixture {
    QList<QRectF> obstacles;
    QRectF bounds;
    QPointF S_left;
    QPointF E_tip;
    QPointF E_deep;
    qreal gap = 28.0;

    ChainFixture()
    {
        obstacles = {
            QRectF(100, 40, 80, 40),
            QRectF(190, 40, 80, 40),
            QRectF(280, 40, 80, 40),
            QRectF(370, 40, 80, 40)
        };
        bounds = obstacles.first();
        for(const QRectF& r : obstacles)
            bounds = bounds.united(r);
        S_left = QPointF(60, 60);
        E_tip = QPointF(100, 60);
        E_deep = QPointF(370, 60);
    }
};

bool firstSegmentLeavesRight(const QPainterPath& path, const QPointF& start)
{
    if(path.elementCount() < 2)
        return false;
    const QPainterPath::Element e0 = path.elementAt(0);
    const QPainterPath::Element e1 = path.elementAt(1);
    return qAbs(e0.y - start.y()) < 1e-3
        && qAbs(e0.x - start.x()) < 1e-3
        && e1.x > start.x() + 1.0
        && qAbs(e1.y - start.y()) < 1e-3;
}

bool lastSegmentArrivesFromLeft(const QPainterPath& path, const QPointF& end)
{
    if(path.elementCount() < 2)
        return false;
    const QPainterPath::Element eLast = path.elementAt(path.elementCount() - 1);
    const QPainterPath::Element ePrev = path.elementAt(path.elementCount() - 2);
    return qAbs(eLast.x - end.x()) < 1e-3
        && qAbs(eLast.y - end.y()) < 1e-3
        && ePrev.x < end.x() - 1.0
        && qAbs(ePrev.y - end.y()) < 1e-3;
}

} // namespace

TEST(UModernDiagramLinkRouter, AutoExternal_UsesCorridor)
{
    ChainFixture f;
    RouteRequest req;
    req.start = f.S_left;
    req.end = f.E_deep;
    req.obstacles = f.obstacles;
    req.diagramBounds = f.bounds;
    req.externalIncoming = true;
    req.cornerRadius = 0.0;
    const RouteResult rr = route(req, RouteMode::Auto);
    EXPECT_EQ(rr.modeUsed, RouteMode::ExternalCorridor);
    EXPECT_EQ(rr.obstacleHits, 0);
}

TEST(UModernDiagramLinkRouter, ExternalDeep_OutsideEnvelopeClearsChain)
{
    ChainFixture f;
    // Port below the row — bottom envelope should clear all boxes
    RouteRequest req;
    req.start = QPointF(60, f.bounds.bottom() + f.gap);
    req.end = f.E_deep;
    req.obstacles = f.obstacles;
    req.diagramBounds = f.bounds;
    req.externalIncoming = true;
    req.cornerRadius = 0.0;
    const RouteResult rr = route(req, RouteMode::ExternalCorridor);
    EXPECT_EQ(rr.modeUsed, RouteMode::ExternalCorridor);
    EXPECT_EQ(rr.obstacleHits, 0);
    EXPECT_TRUE(pathHasHorizontalPortEnds(rr.path));
}

TEST(UModernDiagramLinkRouter, AutoForward_Cubic)
{
    RouteRequest req;
    req.start = QPointF(100, 60);
    req.end = QPointF(200, 60);
    req.externalIncoming = false;
    const RouteResult rr = route(req, RouteMode::Auto);
    EXPECT_EQ(rr.modeUsed, RouteMode::CubicFallback);
}

TEST(UModernDiagramLinkRouter, AutoReverse_OrthogonalFromOutput)
{
    ChainFixture f;
    const QPointF S(400, 60);
    const QPointF E(150, 60);
    RouteRequest req;
    req.start = S;
    req.end = E;
    req.obstacles = f.obstacles;
    req.diagramBounds = f.bounds;
    req.externalIncoming = false;
    req.cornerRadius = 0.0;

    EXPECT_TRUE(isReverseLink(S, E));
    const RouteResult rr = route(req, RouteMode::Auto);
    EXPECT_EQ(rr.modeUsed, RouteMode::OrthogonalAvoid);
    EXPECT_TRUE(pathHasHorizontalPortEnds(rr.path));
    EXPECT_TRUE(firstSegmentLeavesRight(rr.path, S));
    EXPECT_TRUE(lastSegmentArrivesFromLeft(rr.path, E));
    EXPECT_EQ(rr.obstacleHits, 0);
}

TEST(UModernDiagramLinkRouter, MidRowPort_WorseThanOutsideForDeep)
{
    ChainFixture f;
    RouteRequest mid;
    mid.start = f.S_left; // y aligned with row
    mid.end = f.E_deep;
    mid.obstacles = f.obstacles;
    mid.diagramBounds = f.bounds;
    mid.externalIncoming = true;
    mid.cornerRadius = 0.0;

    RouteRequest outside = mid;
    outside.start = QPointF(f.S_left.x(), f.bounds.bottom() + f.gap);

    const RouteResult midRr = route(mid, RouteMode::ExternalCorridor);
    const RouteResult outRr = route(outside, RouteMode::ExternalCorridor);
    // Outside should not hit; mid row left-pocket H at E.y typically hits or scores worse
    EXPECT_EQ(outRr.obstacleHits, 0);
    EXPECT_LE(outRr.obstacleHits, midRr.obstacleHits);
}

TEST(UModernDiagramLinkRouter, LeftPocketFanOut_UsesVerticalBusNotEnvelope)
{
    // Tall column of tip targets (like 10 membrane synapses) + left virtual port
    QList<QRectF> obstacles;
    for(int i = 0; i < 10; ++i)
        obstacles.append(QRectF(100, 40 + i * 50, 80, 36));
    QRectF bounds = obstacles.first();
    for(const QRectF& r : obstacles)
        bounds = bounds.united(r);

    const QPointF S(60, bounds.center().y());
    int totalBends = 0;
    int totalHits = 0;
    for(int i = 0; i < 10; ++i)
    {
        const QPointF E(obstacles[i].left(), obstacles[i].center().y());
        RouteRequest req;
        req.start = S;
        req.end = E;
        req.obstacles = obstacles;
        req.diagramBounds = bounds;
        req.externalIncoming = true;
        req.parallelIndex = i;
        req.cornerRadius = 0.0;
        const RouteResult rr = route(req, RouteMode::ExternalCorridor);
        EXPECT_EQ(rr.modeUsed, RouteMode::ExternalCorridor);
        EXPECT_EQ(rr.obstacleHits, 0) << "target " << i;
        totalHits += rr.obstacleHits;
        totalBends += rr.bendCount;
        // Left-pocket bus: at most one bend (V then H), not full top/bottom envelope
        EXPECT_LE(rr.bendCount, 2) << "target " << i;
        EXPECT_TRUE(pathHasHorizontalPortEnds(rr.path));
    }
    EXPECT_EQ(totalHits, 0);
    EXPECT_LE(totalBends, 20); // 10 links × ≤2 bends; envelopes would be ~30+
}
