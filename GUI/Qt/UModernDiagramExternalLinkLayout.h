#ifndef UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
#define UMODERNDIAGRAMEXTERNALLINKLAYOUT_H

#include "UModernDiagramPort.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramLinkRouter.h"
#include "UModernDiagramExternalSinkLayout.h"

#include <QtGlobal>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QList>
#include <algorithm>

struct ExternalLayoutTarget {
    UModernDiagramNodeItem* dstNode = nullptr;
    PortCategory dstCategory = PortCategory::Own;
};

struct ExternalSourceStoredPosition {
    QPointF pos;
    bool manual = false;
};

namespace ExternalLinkLayout {

using ExternalSinkLayout::sinkTopLeftBesideSource;
using ExternalSinkLayout::isPlausibleSinkTopLeft;

/// Score a candidate virtual-port position. Uses ExternalCorridor (same as live Auto
/// for dashed links) so obstacleHits/bends drive placement.
/// When the port is already in the left pocket of a tall fan-out, prefer staying
/// aligned with target Y (shared vertical bus) instead of forcing outside-band Y.
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

    const bool inLeftPocket = !nodesBounds.isNull() && !nodesBounds.isEmpty()
        && portScenePos.x() < nodesBounds.left() - 1.0;

    // Outside-band bias helps deep L→R chains, but for a left-pocket fan-out to a
    // vertical column it pushes the port above/below and forces envelope kinks.
    if(!nodesBounds.isNull() && !nodesBounds.isEmpty() && !inLeftPocket)
    {
        if(portScenePos.y() >= nodesBounds.top() && portScenePos.y() <= nodesBounds.bottom())
            score += 400;
    }

    QList<qreal> targetYs;
    targetYs.reserve(targets.size());
    for(int ti = 0; ti < targets.size(); ++ti)
    {
        const ExternalLayoutTarget& t = targets[ti];
        if(!t.dstNode)
            continue;
        const QPointF end = t.dstNode->scenePortPosByCategory(false, t.dstCategory);
        targetYs.append(end.y());

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
        // Mild pull toward each target; median alignment handled via candidates
        score += static_cast<int>(qAbs(portScenePos.y() - end.y()) * 0.15);
    }

    // Reward staying near the median target Y for multi-fan (shared bus)
    if(targetYs.size() >= 2)
    {
        std::sort(targetYs.begin(), targetYs.end());
        const qreal medianY = targetYs[targetYs.size() / 2];
        score += static_cast<int>(qAbs(portScenePos.y() - medianY) * 0.5);
    }
    return score;
}

} // namespace ExternalLinkLayout

#endif // UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
