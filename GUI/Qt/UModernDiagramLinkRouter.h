#ifndef UMODERNDIAGRAMLINKROUTER_H
#define UMODERNDIAGRAMLINKROUTER_H

#include <QList>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QtGlobal>

/// Lightweight Manhattan / corridor link router for Modern Diagram.
/// Inspired by Orthogonal Connector Routing (Wybrow et al.) / libavoid ideas,
/// without vendoring libavoid.
namespace UModernDiagramLinkRouter {

enum class RouteMode {
    Auto,              // reverse → OrthogonalAvoid; else cubic (incl. external dashed)
    ExternalCorridor,  // optional outside-envelope (explicit mode only)
    OrthogonalAvoid,   // H-ended Manhattan for reverse links only
    CubicFallback      // classic cubic Bezier
};

struct RouteRequest {
    QPointF start;
    QPointF end;
    QList<QRectF> obstacles; // scene coords (may already be inflated)
    QRectF diagramBounds;    // united node bounds (may be null/empty)
    bool externalIncoming = false;
    qreal corridorGap = 28.0;
    qreal cornerRadius = 6.0;
    int parallelIndex = 0;
    qreal parallelStep = 6.0;
};

struct RouteResult {
    QPainterPath path;
    int bendCount = 0;
    qreal length = 0;
    int obstacleHits = 0;
    RouteMode modeUsed = RouteMode::CubicFallback;
};

inline bool isReverseLink(const QPointF& start, const QPointF& end, qreal eps = 8.0)
{
    return start.x() > end.x() + eps;
}

RouteResult route(const RouteRequest& req, RouteMode mode = RouteMode::Auto);

int obstacleHitsOnPolyline(const QVector<QPointF>& pts,
                           const QList<QRectF>& obstacles,
                           int samplesPerSeg = 8);
qreal polylineLength(const QVector<QPointF>& pts);
int countBends(const QVector<QPointF>& pts);

/// True if first and last path segments are horizontal (side-port invariant).
bool pathHasHorizontalPortEnds(const QPainterPath& path);

} // namespace UModernDiagramLinkRouter

#endif // UMODERNDIAGRAMLINKROUTER_H
