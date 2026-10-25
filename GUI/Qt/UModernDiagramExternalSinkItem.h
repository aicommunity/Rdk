#ifndef UMODERNDIAGRAMEXTERNALSINKITEM_H
#define UMODERNDIAGRAMEXTERNALSINKITEM_H

#include "UModernDiagramExternalLinkLayout.h"
#include "UModernDiagramPort.h"

#include <QGraphicsItem>
#include <QString>

class UModernDiagramWidget;
class UModernDiagramNodeItem;

/// Visual external sink port (local output exported outside current scope) on the right.
class UModernDiagramExternalSinkItem : public QGraphicsItem
{
public:
    UModernDiagramExternalSinkItem(UModernDiagramWidget* owner,
                                   const QString& outputItemId,
                                   const QString& outputItemName);

    QString outputFullId() const;
    QString outputDisplayName() const { return m_outputItemName; }
    QString displayLabel() const { return m_displayLabel; }
    QString outputItemId() const { return m_outputItemId; }
    QString outputItemName() const { return m_outputItemName; }

    void refreshDisplayLabel();

    /// Left edge of diamond (stub arrives from local output on the left).
    QPointF scenePortPos() const;
    void layoutBeside(UModernDiagramNodeItem* srcNode, int stackIndex);
    void layoutOptimal(const QRectF& nodesBounds,
                       const QList<QRectF>& obstacleRects,
                       UModernDiagramNodeItem* srcNode,
                       PortCategory srcCategory,
                       qreal stackOffsetY = 0.0);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    UModernDiagramWidget* m_owner = nullptr;
    QString m_outputItemId;
    QString m_outputItemName;
    QString m_displayLabel;
    mutable QRectF m_cachedBounds;

    static constexpr qreal kPortSize = 8.0;
    static constexpr qreal kGapFromNode = 28.0;
    static constexpr qreal kStackStep = 22.0;
    static constexpr qreal kLabelMaxWidth = 280.0;
    static constexpr qreal kLabelGap = 6.0;
};

#endif // UMODERNDIAGRAMEXTERNALSINKITEM_H
