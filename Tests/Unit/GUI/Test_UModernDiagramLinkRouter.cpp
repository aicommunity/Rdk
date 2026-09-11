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

TEST(UModernDiagramLinkRouter, AutoExternal_UsesCubicNotManhattan)
{
    ChainFixture f;
    RouteRequest req;
    req.start = f.S_left;
    req.end = f.E_deep;
    req.obstacles = f.obstacles;
    req.diagramBounds = f.bounds;
    req.externalIncoming = true;
    const RouteResult rr = route(req, RouteMode::Auto);
    EXPECT_EQ(rr.modeUsed, RouteMode::CubicFallback);
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
    // LTZone-like source on the right, Soma-like target on the left
    const QPointF S(400, 60); // output on right edge of right node
    const QPointF E(150, 60); // input on left edge of left node
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

TEST(UModernDiagramLinkRouter, ReverseExplicit_LeavesOutputRightward)
{
    const QPointF S(300, 50);
    const QPointF E(100, 50);
    QList<QRectF> obstacles = { QRectF(120, 30, 60, 40), QRectF(200, 30, 80, 40) };
    QRectF bounds = obstacles[0].united(obstacles[1]);
    bounds = bounds.united(QRectF(S, E).normalized());

    RouteRequest req;
    req.start = S;
    req.end = E;
    req.obstacles = obstacles;
    req.diagramBounds = bounds;
    req.cornerRadius = 0.0;

    const RouteResult rr = route(req, RouteMode::OrthogonalAvoid);
    EXPECT_EQ(rr.modeUsed, RouteMode::OrthogonalAvoid);
    EXPECT_TRUE(firstSegmentLeavesRight(rr.path, S));
    EXPECT_TRUE(lastSegmentArrivesFromLeft(rr.path, E));
}

TEST(UModernDiagramLinkRouter, ExternalCorridorExplicit_StillAvailable)
{
    ChainFixture f;
    RouteRequest req;
    req.start = f.S_left;
    req.end = f.E_deep;
    req.obstacles = f.obstacles;
    req.diagramBounds = f.bounds;
    req.externalIncoming = true;
    req.cornerRadius = 0.0;
    const RouteResult rr = route(req, RouteMode::ExternalCorridor);
    EXPECT_EQ(rr.modeUsed, RouteMode::ExternalCorridor);
}
