#ifndef UMODERNDIAGRAMEXTERNALSINKLAYOUT_H
#define UMODERNDIAGRAMEXTERNALSINKLAYOUT_H

#include <QtGlobal>
#include <QPointF>
#include <QRectF>
#include <QSizeF>

/// Pure right-side sink placement (no Qt Widgets — unit-testable).
namespace ExternalSinkLayout {

/// Top-left of a right-side sink item: diamond on the left edge of the item,
/// immediately to the right of the local source node (not in the left input pocket).
inline QPointF sinkTopLeftBesideSource(const QRectF& srcSceneRect,
                                       const QSizeF& itemSize,
                                       int stackIndex = 0,
                                       qreal gapFromNode = 28.0,
                                       qreal stackStep = 22.0)
{
    const qreal centerY = srcSceneRect.center().y() + stackIndex * stackStep;
    const qreal portX = srcSceneRect.right() + gapFromNode;
    return QPointF(portX, centerY - itemSize.height() / 2.0);
}

/// Auto-restored sink positions must stay on the right of the source (or diagram).
inline bool isPlausibleSinkTopLeft(const QPointF& topLeft,
                                   const QRectF& srcSceneRect,
                                   const QRectF& nodesBounds,
                                   qreal slop = 8.0)
{
    const qreal minX = srcSceneRect.isNull()
        ? (nodesBounds.isNull() ? topLeft.x() : nodesBounds.right() - slop)
        : srcSceneRect.right() - slop;
    return topLeft.x() >= minX;
}

} // namespace ExternalSinkLayout

#endif // UMODERNDIAGRAMEXTERNALSINKLAYOUT_H
