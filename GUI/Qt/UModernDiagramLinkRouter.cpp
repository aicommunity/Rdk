#include "UModernDiagramLinkRouter.h"

#include <QLineF>
#include <QtGlobal>
#include <algorithm>
#include <cmath>
#include <limits>

namespace UModernDiagramLinkRouter {
namespace {

constexpr qreal kEps = 1e-3;
constexpr qreal kPortStub = 8.0; // short side-port approach (fits tight module gaps)

QList<QRectF> inflateRects(const QList<QRectF>& rects, qreal margin)
{
    QList<QRectF> out;
    out.reserve(rects.size());
    for(const QRectF& r : rects)
        out.append(r.adjusted(-margin, -margin, margin, margin));
    return out;
}

QList<QRectF> filterEndpoints(const QList<QRectF>& obstacles,
                              const QPointF& start,
                              const QPointF& end)
{
    QList<QRectF> out;
    out.reserve(obstacles.size());
    for(const QRectF& r : obstacles)
    {
        if(r.contains(start) || r.contains(end))
            continue;
        out.append(r);
    }
    return out;
}

int segmentHits(const QPointF& a, const QPointF& b,
                const QList<QRectF>& obstacles, int samples)
{
    int hits = 0;
    const int n = qMax(1, samples);
    for(int i = 1; i <= n; ++i)
    {
        const qreal t = i / qreal(n);
        const QPointF p(a.x() + (b.x() - a.x()) * t,
                        a.y() + (b.y() - a.y()) * t);
        for(const QRectF& r : obstacles)
        {
            if(r.contains(p))
            {
                ++hits;
                break;
            }
        }
    }
    return hits;
}

QVector<QPointF> orthogonalizePoints(const QVector<QPointF>& pts)
{
    if(pts.size() < 2)
        return pts;

    QVector<QPointF> out;
    out.reserve(pts.size() * 2);
    out.append(pts.first());
    for(int i = 1; i < pts.size(); ++i)
    {
        const QPointF prev = out.last();
        const QPointF cur = pts[i];
        const qreal dx = qAbs(cur.x() - prev.x());
        const qreal dy = qAbs(cur.y() - prev.y());
        if(dx < kEps || dy < kEps)
        {
            out.append(cur);
        }
        else
        {
            out.append(QPointF(cur.x(), prev.y()));
            out.append(cur);
        }
    }
    return out;
}

QVector<QPointF> collapseColinear(const QVector<QPointF>& pts)
{
    if(pts.size() < 3)
        return pts;

    QVector<QPointF> out;
    out.reserve(pts.size());
    out.append(pts.first());
    for(int i = 1; i + 1 < pts.size(); ++i)
    {
        const QPointF& a = out.last();
        const QPointF& b = pts[i];
        const QPointF& c = pts[i + 1];
        const bool colinearH = qAbs(a.y() - b.y()) < kEps && qAbs(b.y() - c.y()) < kEps;
        const bool colinearV = qAbs(a.x() - b.x()) < kEps && qAbs(b.x() - c.x()) < kEps;
        if(colinearH || colinearV)
            continue;
        if(QLineF(a, b).length() < kEps)
            continue;
        out.append(b);
    }
    if(QLineF(out.last(), pts.last()).length() >= kEps)
        out.append(pts.last());
    else if(out.size() == 1)
        out.append(pts.last());
    return out;
}

QVector<QPointF> ensureSidePortEnds(QVector<QPointF> pts,
                                    const QPointF& S,
                                    const QPointF& E,
                                    qreal stub = kPortStub)
{
    if(pts.size() < 2)
        return pts;

    pts[0] = S;
    pts[pts.size() - 1] = E;

    // Output ports sit on the right edge — leave to +X, never through the node body.
    {
        const bool leavesRight = pts.size() >= 2
            && qAbs(pts[1].y() - S.y()) < kEps
            && pts[1].x() > S.x() + kEps;
        if(!leavesRight)
        {
            const QPointF stubPt(S.x() + stub, S.y());
            const bool nextWasVerticalAtS = qAbs(pts[1].x() - S.x()) < kEps;
            pts.insert(1, stubPt);
            if(nextWasVerticalAtS && pts.size() > 2)
                pts[2] = QPointF(stubPt.x(), pts[2].y());
        }
    }

    // Input ports sit on the left edge — approach from -X.
    {
        const int n = pts.size();
        const bool arrivesFromLeft = n >= 2
            && qAbs(pts[n - 2].y() - E.y()) < kEps
            && pts[n - 2].x() < E.x() - kEps;
        if(!arrivesFromLeft)
        {
            const QPointF stubPt(E.x() - stub, E.y());
            if(n >= 2 && qAbs(pts[n - 2].x() - E.x()) < kEps)
                pts[n - 2] = QPointF(stubPt.x(), pts[n - 2].y());
            pts.insert(pts.size() - 1, stubPt);
        }
    }

    return collapseColinear(orthogonalizePoints(pts));
}

bool hasHorizontalPortEnds(const QVector<QPointF>& pts)
{
    if(pts.size() < 2)
        return false;
    const bool exitH = qAbs(pts[0].y() - pts[1].y()) < kEps;
    const bool enterH = qAbs(pts[pts.size() - 2].y() - pts[pts.size() - 1].y()) < kEps;
    return exitH && enterH;
}

QVector<QPointF> normalizeCandidate(const QVector<QPointF>& pts,
                                    const QPointF& S,
                                    const QPointF& E)
{
    return ensureSidePortEnds(collapseColinear(orthogonalizePoints(pts)), S, E);
}

QPainterPath pathFromPoints(const QVector<QPointF>& pts, qreal cornerRadius)
{
    QPainterPath path;
    if(pts.isEmpty())
        return path;
    path.moveTo(pts.first());
    if(pts.size() == 1)
        return path;

    if(cornerRadius <= 0.0 || pts.size() < 3)
    {
        for(int i = 1; i < pts.size(); ++i)
            path.lineTo(pts[i]);
        return path;
    }

    for(int i = 1; i + 1 < pts.size(); ++i)
    {
        const QPointF prev = pts[i - 1];
        const QPointF corner = pts[i];
        const QPointF next = pts[i + 1];
        const QLineF in(corner, prev);
        const QLineF out(corner, next);
        const qreal inLen = in.length();
        const qreal outLen = out.length();
        if(inLen < kEps || outLen < kEps)
        {
            path.lineTo(corner);
            continue;
        }
        const qreal r = qMin(cornerRadius, qMin(inLen, outLen) * 0.45);
        const QPointF p1 = corner + (prev - corner) * (r / inLen);
        const QPointF p2 = corner + (next - corner) * (r / outLen);
        path.lineTo(p1);
        path.quadTo(corner, p2);
    }
    path.lineTo(pts.last());
    return path;
}

qreal horizontalInteriorLength(const QPointF& a, const QPointF& b, const QRectF& bounds)
{
    if(qAbs(a.y() - b.y()) > kEps)
        return 0.0;
    const qreal y = a.y();
    if(y < bounds.top() - kEps || y > bounds.bottom() + kEps)
        return 0.0;

    const qreal x0 = qMin(a.x(), b.x());
    const qreal x1 = qMax(a.x(), b.x());
    const qreal overlap0 = qMax(x0, bounds.left());
    const qreal overlap1 = qMin(x1, bounds.right());
    return qMax(0.0, overlap1 - overlap0);
}

qreal scorePolyline(const QVector<QPointF>& pts,
                    const QList<QRectF>& obstacles,
                    const QRectF& bounds,
                    qreal corridorGap,
                    bool applyStemPenalty)
{
    const int hits = obstacleHitsOnPolyline(pts, obstacles);
    const int bends = countBends(pts);
    const qreal len = polylineLength(pts);
    qreal stemPenalty = 0.0;
    qreal interiorPenalty = 0.0;
    const QPointF endPt = pts.isEmpty() ? QPointF() : pts.last();
    const qreal nearEndSlack = kPortStub * 3.0;

    if(!bounds.isNull() && !bounds.isEmpty())
    {
        for(int i = 0; i + 1 < pts.size(); ++i)
        {
            const QPointF& a = pts[i];
            const QPointF& b = pts[i + 1];
            if(qAbs(a.x() - b.x()) < kEps)
            {
                // Mid-chain vertical stem inside diagram — not near-target stub
                if(applyStemPenalty
                   && a.x() > bounds.left() - corridorGap * 0.5
                   && qAbs(a.x() - endPt.x()) > nearEndSlack)
                {
                    stemPenalty += 500.0;
                }
            }
            else if(qAbs(a.y() - b.y()) < kEps)
            {
                const qreal interiorLen = horizontalInteriorLength(a, b, bounds);
                if(interiorLen > 40.0)
                    interiorPenalty += interiorLen * 2.0;
            }
        }
    }

    const qreal portPenalty = hasHorizontalPortEnds(pts) ? 0.0 : 1e6;
    return hits * 1000.0 + bends * 40.0 + len * 0.05
           + stemPenalty + interiorPenalty + portPenalty;
}

RouteResult makeResult(const QVector<QPointF>& pts,
                       const QList<QRectF>& obstacles,
                       qreal cornerRadius,
                       RouteMode mode)
{
    RouteResult result;
    result.path = pathFromPoints(pts, cornerRadius);
    result.bendCount = countBends(pts);
    result.length = polylineLength(pts);
    result.obstacleHits = obstacleHitsOnPolyline(pts, obstacles);
    result.modeUsed = mode;
    return result;
}

RouteResult routeCubicFallback(const RouteRequest& req)
{
    QPainterPath path(req.start);
    const qreal dx = req.end.x() - req.start.x();
    path.cubicTo(req.start + QPointF(dx * 0.4, 0),
                 req.end - QPointF(dx * 0.4, 0),
                 req.end);
    RouteResult result;
    result.path = path;
    result.bendCount = 0;
    result.length = QLineF(req.start, req.end).length();
    result.obstacleHits = 0;
    result.modeUsed = RouteMode::CubicFallback;
    return result;
}

QVector<QPointF> routeAroundBox(const QPointF& s, const QPointF& e,
                                const QRectF& box, int side)
{
    const qreal stub = kPortStub;
    const qreal stubX = e.x() - stub;
    switch(side)
    {
    case 0:
    {
        const qreal y = box.top();
        return {s, QPointF(s.x(), y), QPointF(stubX, y), QPointF(stubX, e.y()), e};
    }
    case 1:
    {
        const qreal y = box.bottom();
        return {s, QPointF(s.x(), y), QPointF(stubX, y), QPointF(stubX, e.y()), e};
    }
    case 2:
    {
        const qreal x = qMin(box.left(), stubX);
        return {s, QPointF(x, s.y()), QPointF(x, e.y()), e};
    }
    default:
    {
        const qreal x = box.right();
        return {s, QPointF(x, s.y()), QPointF(x, e.y()), QPointF(stubX, e.y()), e};
    }
    }
}

QPointF projectOrtho(const QPointF& from, const QPointF& to)
{
    return QPointF(to.x(), from.y());
}

QList<QRectF> nearestObstacles(const QPointF& s,
                               const QList<QRectF>& obstacles,
                               int k)
{
    struct Item { qreal d; QRectF r; };
    QVector<Item> items;
    items.reserve(obstacles.size());
    for(const QRectF& r : obstacles)
    {
        const QPointF c = r.center();
        items.append({QLineF(s, c).length(), r});
    }
    std::sort(items.begin(), items.end(),
              [](const Item& a, const Item& b) { return a.d < b.d; });
    QList<QRectF> out;
    const int n = qMin(k, items.size());
    for(int i = 0; i < n; ++i)
        out.append(items[i].r);
    return out;
}

QVector<QPointF> inflatedCorners(const QRectF& o, qreal gap)
{
    const QRectF r = o.adjusted(-gap, -gap, gap, gap);
    return {
        r.topLeft(),
        r.topRight(),
        r.bottomLeft(),
        r.bottomRight()
    };
}

RouteResult routeExternalCorridor(const RouteRequest& req)
{
    const QPointF S = req.start;
    const QPointF E = req.end;
    const QRectF bounds = req.diagramBounds;
    if(bounds.isNull() || bounds.isEmpty())
        return routeCubicFallback(req);

    const qreal gap = req.corridorGap;
    const qreal stub = kPortStub;
    const qreal stubX = E.x() - stub;
    const qreal leftPocketX = bounds.left() - gap
                              - req.parallelIndex * req.parallelStep;
    const bool inLeftPocket = S.x() <= bounds.left() - 1.0;

    const QList<QRectF> obstacles = inflateRects(
        filterEndpoints(req.obstacles, S, E), 0.0);

    QVector<QVector<QPointF>> candidates;

    // Left-pocket bus: shared vertical stem at S.x, then short H into the target.
    // Preferred for fan-out to a column of synapses (avoids full-bounds envelope).
    if(inLeftPocket)
    {
        candidates.append({S, QPointF(S.x(), E.y()), E});
        // Soft vertical offset lanes when several links share one port
        if(req.parallelIndex > 0)
        {
            const qreal laneX = S.x() - req.parallelIndex * req.parallelStep;
            candidates.append({S, QPointF(laneX, S.y()), QPointF(laneX, E.y()), E});
        }
    }
    else
    {
        candidates.append({S,
                           QPointF(leftPocketX, S.y()),
                           QPointF(leftPocketX, E.y()),
                           E});
    }

    // Outside envelope — only needed when the port is not already clear on the left
    // (or left-stem is blocked). Heavily penalized via scoring when inLeftPocket.
    const qreal topY = bounds.top() - gap - req.parallelIndex * req.parallelStep;
    const qreal botY = bounds.bottom() + gap + req.parallelIndex * req.parallelStep;
    candidates.append({S, QPointF(S.x(), topY), QPointF(stubX, topY), QPointF(stubX, E.y()), E});
    candidates.append({S, QPointF(S.x(), botY), QPointF(stubX, botY), QPointF(stubX, E.y()), E});

    QVector<QPointF> best;
    qreal bestScore = std::numeric_limits<qreal>::infinity();
    for(const QVector<QPointF>& raw : candidates)
    {
        QVector<QPointF> pts = normalizeCandidate(raw, S, E);
        if(pts.size() < 2)
            continue;
        qreal score = scorePolyline(pts, obstacles, bounds, gap, true);
        // Prefer left-pocket bus over full envelope when the port is already clear left
        if(inLeftPocket)
        {
            const bool looksLikeEnvelope = pts.size() >= 4
                && (qAbs(pts[1].x() - S.x()) < kEps)
                && (pts[1].y() < bounds.top() - gap * 0.5
                    || pts[1].y() > bounds.bottom() + gap * 0.5);
            if(looksLikeEnvelope)
                score += 2500.0;
        }
        if(score < bestScore)
        {
            bestScore = score;
            best = pts;
        }
    }

    if(best.size() < 2)
        return routeCubicFallback(req);
    return makeResult(best, obstacles, req.cornerRadius, RouteMode::ExternalCorridor);
}

RouteResult routeOrthogonalAvoid(const RouteRequest& req)
{
    const QPointF S = req.start;
    const QPointF E = req.end;
    QList<QRectF> obstacles = inflateRects(
        filterEndpoints(req.obstacles, S, E), 4.0);

    const qreal dx = E.x() - S.x();
    const qreal dy = E.y() - S.y();
    if(qAbs(dx) < 2.0 && qAbs(dy) < 2.0)
    {
        return makeResult({S, E}, obstacles, 0.0, RouteMode::OrthogonalAvoid);
    }

    const qreal stub = kPortStub;
    const qreal outX = S.x() + stub;
    const qreal inX = E.x() - stub;

    QRectF U = req.diagramBounds;
    if(U.isNull() || U.isEmpty())
    {
        U = QRectF(S, E).normalized();
        for(const QRectF& o : obstacles)
            U = U.united(o);
    }
    const qreal gap = req.corridorGap;
    const QRectF pocket = U.adjusted(-gap, -gap, gap, gap);

    // Prefer paths that leave the output to the right, then clear the node row
    QVector<QVector<QPointF>> candidates;
    candidates.append({S, QPointF(outX, S.y()), QPointF(outX, pocket.top()),
                       QPointF(inX, pocket.top()), QPointF(inX, E.y()), E});
    candidates.append({S, QPointF(outX, S.y()), QPointF(outX, pocket.bottom()),
                       QPointF(inX, pocket.bottom()), QPointF(inX, E.y()), E});
    candidates.append({S, QPointF(outX, S.y()), QPointF(outX, E.y()), E});
    candidates.append({S, QPointF(outX, S.y()), QPointF(outX, S.y() + dy * 0.5),
                       QPointF(inX, S.y() + dy * 0.5), QPointF(inX, E.y()), E});

    for(int side = 0; side < 4; ++side)
        candidates.append(routeAroundBox(S, E, pocket, side));

    const QList<QRectF> nearest = nearestObstacles(S, obstacles, 3);
    for(const QRectF& o : nearest)
    {
        for(const QPointF& corner : inflatedCorners(o, 12.0))
        {
            candidates.append({S, QPointF(outX, S.y()), corner,
                               QPointF(inX, corner.y()), QPointF(inX, E.y()), E});
        }
    }

    QVector<QPointF> best;
    qreal bestScore = std::numeric_limits<qreal>::infinity();
    for(const QVector<QPointF>& raw : candidates)
    {
        QVector<QPointF> pts = normalizeCandidate(raw, S, E);
        if(pts.size() < 2 || !hasHorizontalPortEnds(pts))
            continue;
        // Reject paths that leave the output leftward into the node
        if(pts[1].x() <= S.x() + kEps)
            continue;
        const qreal score = scorePolyline(pts, obstacles, U, gap, false);
        if(score < bestScore)
        {
            bestScore = score;
            best = pts;
        }
    }

    // Reverse links must stay Manhattan — never fall back to L→R cubic (wrong port tangents)
    if(best.size() >= 2)
        return makeResult(best, obstacles, req.cornerRadius, RouteMode::OrthogonalAvoid);

    // Last resort: explicit top envelope from output
    QVector<QPointF> fallback = normalizeCandidate(
        {S, QPointF(outX, S.y()), QPointF(outX, pocket.top()),
         QPointF(inX, pocket.top()), QPointF(inX, E.y()), E},
        S, E);
    if(fallback.size() >= 2)
        return makeResult(fallback, obstacles, req.cornerRadius, RouteMode::OrthogonalAvoid);
    return makeResult({S, QPointF(outX, S.y()), QPointF(outX, E.y()), E},
                      obstacles, req.cornerRadius, RouteMode::OrthogonalAvoid);
}

} // namespace

int obstacleHitsOnPolyline(const QVector<QPointF>& pts,
                           const QList<QRectF>& obstacles,
                           int samplesPerSeg)
{
    if(pts.size() < 2 || obstacles.isEmpty())
        return 0;
    int hits = 0;
    for(int i = 0; i + 1 < pts.size(); ++i)
        hits += segmentHits(pts[i], pts[i + 1], obstacles, samplesPerSeg);
    return hits;
}

qreal polylineLength(const QVector<QPointF>& pts)
{
    qreal len = 0.0;
    for(int i = 0; i + 1 < pts.size(); ++i)
        len += QLineF(pts[i], pts[i + 1]).length();
    return len;
}

int countBends(const QVector<QPointF>& pts)
{
    if(pts.size() < 3)
        return 0;
    int bends = 0;
    for(int i = 1; i + 1 < pts.size(); ++i)
    {
        const QPointF d0 = pts[i] - pts[i - 1];
        const QPointF d1 = pts[i + 1] - pts[i];
        const bool h0 = qAbs(d0.y()) < kEps;
        const bool h1 = qAbs(d1.y()) < kEps;
        if(h0 != h1)
            ++bends;
    }
    return bends;
}

bool pathHasHorizontalPortEnds(const QPainterPath& path)
{
    if(path.elementCount() < 2)
        return false;
    const QPainterPath::Element e0 = path.elementAt(0);
    const QPainterPath::Element e1 = path.elementAt(1);
    const QPainterPath::Element eLast = path.elementAt(path.elementCount() - 1);
    const QPainterPath::Element ePrev = path.elementAt(path.elementCount() - 2);
    return qAbs(e0.y - e1.y) < kEps && qAbs(ePrev.y - eLast.y) < kEps;
}

RouteResult route(const RouteRequest& req, RouteMode mode)
{
    RouteMode effective = mode;
    if(effective == RouteMode::Auto)
    {
        // Dashed external → corridor (avoid crossing the node row).
        // Reverse internal → Manhattan from output (+X) to input (−X).
        // Forward internal → classic cubic.
        if(req.externalIncoming)
            effective = RouteMode::ExternalCorridor;
        else if(isReverseLink(req.start, req.end))
            effective = RouteMode::OrthogonalAvoid;
        else
            effective = RouteMode::CubicFallback;
    }

    switch(effective)
    {
    case RouteMode::ExternalCorridor:
        return routeExternalCorridor(req);
    case RouteMode::OrthogonalAvoid:
        return routeOrthogonalAvoid(req);
    case RouteMode::CubicFallback:
        return routeCubicFallback(req);
    case RouteMode::Auto:
    default:
        return routeCubicFallback(req);
    }
}

} // namespace UModernDiagramLinkRouter
