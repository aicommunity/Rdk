#ifndef UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
#define UMODERNDIAGRAMEXTERNALLINKLAYOUT_H

#include "UModernDiagramPort.h"
#include "UModernDiagramNodeItem.h"

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

inline QPainterPath cubicLinkPath(const QPointF& start, const QPointF& end)
{
    QPainterPath path(start);
    const qreal dx = end.x() - start.x();
    path.cubicTo(start + QPointF(dx * 0.4, 0),
                 end - QPointF(dx * 0.4, 0),
                 end);
    return path;
}

inline int pathObstacleScore(const QPainterPath& path,
                             const QList<QRectF>& obstacles,
                             int samples = 12)
{
    int score = 0;
    for(int i = 1; i <= samples; ++i)
    {
        const QPointF p = path.pointAtPercent(i / qreal(samples));
        for(const QRectF& r : obstacles)
        {
            if(r.contains(p))
                score += 100;
        }
    }
    return score;
}

inline int layoutScore(const QPointF& portScenePos,
                       const QList<ExternalLayoutTarget>& targets,
                       const QList<QRectF>& obstacles,
                       const QRectF& portRect)
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

    for(const ExternalLayoutTarget& t : targets)
    {
        if(!t.dstNode)
            continue;
        const QPointF end = t.dstNode->scenePortPosByCategory(false, t.dstCategory);
        score += pathObstacleScore(cubicLinkPath(portScenePos, end), obstacles);
        score += static_cast<int>(qAbs(portScenePos.y() - end.y()) * 0.2);
    }
    return score;
}

} // namespace ExternalLinkLayout

#endif // UMODERNDIAGRAMEXTERNALLINKLAYOUT_H
