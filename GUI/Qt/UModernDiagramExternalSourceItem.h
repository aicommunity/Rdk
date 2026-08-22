#ifndef UMODERNDIAGRAMEXTERNALSOURCEITEM_H
#define UMODERNDIAGRAMEXTERNALSOURCEITEM_H

#include "UModernDiagramExternalLinkLayout.h"

#include <QGraphicsItem>
#include <QString>

class UModernDiagramWidget;
class UModernDiagramNodeItem;

/// Visual external output port (owner / higher-level source) on the schematic level.
class UModernDiagramExternalSourceItem : public QGraphicsItem
{
public:
    UModernDiagramExternalSourceItem(UModernDiagramWidget* owner,
                                     const QString& sourceItemId,
                                     const QString& sourceItemName);

    QString outputFullId() const;
    QString outputDisplayName() const { return m_sourceItemName; }
    QString displayLabel() const { return m_displayLabel; }
    QString sourceItemId() const { return m_sourceItemId; }
    QString sourceItemName() const { return m_sourceItemName; }

    void refreshDisplayLabel();

    QPointF scenePortPos() const;
    void layoutBeside(UModernDiagramNodeItem* dstNode, int stackIndex);
    void layoutOptimal(const QRectF& nodesBounds,
                       const QList<QRectF>& obstacleRects,
                       const QList<ExternalLayoutTarget>& targets,
                       qreal stackOffsetY = 0.0);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    UModernDiagramWidget* m_owner = nullptr;
    QString m_sourceItemId;
    QString m_sourceItemName;
    QString m_displayLabel;
    mutable QRectF m_cachedBounds;

    static constexpr qreal kPortSize = 8.0;
    static constexpr qreal kGapFromNode = 28.0;
    static constexpr qreal kStackStep = 22.0;
    static constexpr qreal kLabelMaxWidth = 280.0;
    static constexpr qreal kLabelGap = 6.0;
};

#endif // UMODERNDIAGRAMEXTERNALSOURCEITEM_H
