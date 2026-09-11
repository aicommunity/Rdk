#ifndef UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
#define UMODERNDIAGRAMEXTERNALLINKLAYOUT_H

#include "UModernDiagramPort.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramLinkRouter.h"

#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QList>

struct ExternalLayoutTarget {
    UModernDiagramNodeItem* dstNode = nullptr;
    PortCategory dstCategory = PortCategory::Own;
};

struct ExternalSourceStoredPosition {
    QPointF pos;
    bool manual = false;
};

namespace ExternalLinkLayout {

/// Score a candidate virtual-port position. Uses ExternalCorridor (same as live Auto
/// for dashed links) so obstacleHits/bends drive placement above/below the node row.
inline int layoutScore(const QPointF& portScenePos,
                       const QList<ExternalLayoutTarget>& targets,
                       const QList<QRectF>& obstacles,
                       const QRectF& portRect,
                       const QRectF& nodesBounds = QRectF())
{
    int score = 0;
    if(!portRect.isEmpty())
    {
        for(const QRectF& r : obstacles)
        {
            if(portRect.intersects(r))
                score += 1000;
        }
    }

    // Prefer ports outside the horizontal node band (systemic bias, not "always below")
    if(!nodesBounds.isNull() && !nodesBounds.isEmpty())
    {
        if(portScenePos.y() >= nodesBounds.top() && portScenePos.y() <= nodesBounds.bottom())
            score += 400;
    }

    for(int ti = 0; ti < targets.size(); ++ti)
    {
        const ExternalLayoutTarget& t = targets[ti];
        if(!t.dstNode)
            continue;
        const QPointF end = t.dstNode->scenePortPosByCategory(false, t.dstCategory);

        UModernDiagramLinkRouter::RouteRequest req;
        req.start = portScenePos;
        req.end = end;
        req.obstacles = obstacles;
        req.diagramBounds = nodesBounds;
        req.externalIncoming = true;
        req.parallelIndex = ti;

        const UModernDiagramLinkRouter::RouteResult rr =
            UModernDiagramLinkRouter::route(
                req, UModernDiagramLinkRouter::RouteMode::ExternalCorridor);

        score += rr.obstacleHits * 100;
        score += rr.bendCount * 10;
        score += static_cast<int>(rr.length * 0.05);
        score += static_cast<int>(qAbs(portScenePos.y() - end.y()) * 0.2);
    }
    return score;
}

} // namespace ExternalLinkLayout

#endif // UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
