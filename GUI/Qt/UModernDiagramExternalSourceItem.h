#ifndef UMODERNDIAGRAMEXTERNALSOURCEITEM_H
#define UMODERNDIAGRAMEXTERNALSOURCEITEM_H

#include <QGraphicsItem>
#include <QString>

class UModernDiagramWidget;
class UModernDiagramNodeItem;

/// Visual external output port (owner / higher-level source) on the schematic level.
class UModernDiagramExternalSourceItem : public QGraphicsItem
{
public:
    UModernDiagramExternalSourceItem(UModernDiagramWidget* owner,
                                     const QString& outputFullId,
                                     const QString& outputDisplayName);

    QString outputFullId() const { return m_outputFullId; }
    QString outputDisplayName() const { return m_outputDisplayName; }

    QPointF scenePortPos() const;
    void layoutBeside(UModernDiagramNodeItem* dstNode, int stackIndex);
    void layoutOptimal(const QRectF& nodesBounds,
                       const QList<QRectF>& obstacleRects,
                       const QList<UModernDiagramNodeItem*>& targetNodes,
                       qreal stackOffsetY = 0.0);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    UModernDiagramWidget* m_owner = nullptr;
    QString m_outputFullId;
    QString m_outputDisplayName;
    mutable QRectF m_cachedBounds;

    static constexpr qreal kPortSize = 8.0;
    static constexpr qreal kGapFromNode = 28.0;
    static constexpr qreal kStackStep = 22.0;
    static constexpr qreal kLabelMaxWidth = 220.0;
    static constexpr qreal kLabelGap = 6.0;
};

#endif // UMODERNDIAGRAMEXTERNALSOURCEITEM_H
