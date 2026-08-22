#include "UModernDiagramExternalSourceItem.h"

#include "UModernDiagramNodeItem.h"
#include "UModernDiagramWidget.h"
#include "UStyleManager.h"

#include <QPainter>
#include <QFontMetricsF>
#include <QtGlobal>
#include <climits>

UModernDiagramExternalSourceItem::UModernDiagramExternalSourceItem(UModernDiagramWidget* owner,
                                                                   const QString& outputFullId,
                                                                   const QString& outputDisplayName)
    : m_owner(owner)
    , m_outputFullId(outputFullId)
    , m_outputDisplayName(outputDisplayName.isEmpty() ? outputFullId : outputDisplayName)
{
    setZValue(0);
    setAcceptHoverEvents(true);
    setToolTip(m_outputFullId + (m_outputDisplayName != m_outputFullId
        ? QStringLiteral("\n") + m_outputDisplayName
        : QString()));
    prepareGeometryChange();
}

QPointF UModernDiagramExternalSourceItem::scenePortPos() const
{
    return mapToScene(QPointF(boundingRect().right(), boundingRect().center().y()));
}

void UModernDiagramExternalSourceItem::layoutBeside(UModernDiagramNodeItem* dstNode, int stackIndex)
{
    if(!dstNode)
        return;
    const QRectF nr = dstNode->sceneBoundingRect();
    const qreal centerY = nr.center().y() + stackIndex * kStackStep;
    const qreal portX = nr.left() - kGapFromNode;
    setPos(portX - boundingRect().width(), centerY - boundingRect().height() / 2.0);
}

void UModernDiagramExternalSourceItem::layoutOptimal(const QRectF& nodesBounds,
                                                     const QList<QRectF>& obstacleRects,
                                                     const QList<UModernDiagramNodeItem*>& targetNodes,
                                                     qreal stackOffsetY)
{
    if(nodesBounds.isEmpty())
    {
        if(!targetNodes.isEmpty())
            layoutBeside(targetNodes.first(), static_cast<int>(stackOffsetY / kStackStep));
        return;
    }

    const QRectF itemBounds = boundingRect();
    const qreal width = itemBounds.width();
    const qreal height = itemBounds.height();
    const qreal leftX = nodesBounds.left() - kGapFromNode - width;

    QList<qreal> candidateYs;
    candidateYs.reserve(targetNodes.size() + 9);
    qreal targetSumY = 0.0;
    int targetCount = 0;
    for(UModernDiagramNodeItem* dstNode : targetNodes)
    {
        if(!dstNode)
            continue;
        const qreal centerY = dstNode->sceneBoundingRect().center().y();
        candidateYs.append(centerY);
        targetSumY += centerY;
        ++targetCount;
    }
    const qreal avgTargetY = targetCount > 0 ? targetSumY / targetCount : nodesBounds.center().y();
    for(int i = 0; i <= 8; ++i)
        candidateYs.append(nodesBounds.top() + nodesBounds.height() * i / 8.0);

    qreal bestCenterY = avgTargetY + stackOffsetY;
    int bestScore = INT_MAX;
    for(qreal centerY : candidateYs)
    {
        centerY += stackOffsetY;
        const QRectF candidate(leftX, centerY - height / 2.0, width, height);
        int score = 0;
        for(const QRectF& obstacle : obstacleRects)
        {
            if(candidate.intersects(obstacle))
                score += 1000;
        }
        score += static_cast<int>(qAbs(centerY - avgTargetY));
        if(score < bestScore)
        {
            bestScore = score;
            bestCenterY = centerY;
        }
    }

    setPos(leftX, bestCenterY - height / 2.0);
}

QRectF UModernDiagramExternalSourceItem::boundingRect() const
{
    if(!m_cachedBounds.isNull())
        return m_cachedBounds;

    QFont font;
    font.setPointSizeF(8.0);
    QFontMetricsF fm(font);
    const QString label = fm.elidedText(m_outputFullId, Qt::ElideMiddle, kLabelMaxWidth);
    const qreal labelWidth = qMin(kLabelMaxWidth, fm.horizontalAdvance(label));
    const qreal labelHeight = fm.height();
    const qreal width = labelWidth + kLabelGap + kPortSize * 2.0;
    const qreal height = qMax(labelHeight, kPortSize * 2.0) + 4.0;
    m_cachedBounds = QRectF(0, 0, width, height);
    return m_cachedBounds;
}

void UModernDiagramExternalSourceItem::paint(QPainter* painter,
                                             const QStyleOptionGraphicsItem* option,
                                             QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing, true);

    UStyleManager* style = UStyleManager::instance();
    QColor portColor = style ? style->getLinkColor() : QColor(126, 184, 218);
    portColor.setAlpha(220);

    QFont font = painter->font();
    font.setPointSizeF(8.0);
    painter->setFont(font);
    QFontMetricsF fm(font);
    const QString label = fm.elidedText(m_outputFullId, Qt::ElideMiddle, kLabelMaxWidth);
    const qreal labelWidth = qMin(kLabelMaxWidth, fm.horizontalAdvance(label));
    const qreal labelHeight = fm.height();
    const QRectF bounds = boundingRect();
    const qreal centerY = bounds.center().y();

    const QRectF textRect(bounds.left(), centerY - labelHeight / 2.0, labelWidth, labelHeight);
    painter->setPen(style ? style->getTextSecondaryColor() : QColor(180, 180, 180));
    painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);

    const qreal portCenterX = bounds.right() - kPortSize;
    const QPointF center(portCenterX, centerY);
    QPolygonF diamond;
    diamond << center + QPointF(0, -kPortSize)
            << center + QPointF(kPortSize, 0)
            << center + QPointF(0, kPortSize)
            << center + QPointF(-kPortSize, 0);

    painter->setPen(Qt::NoPen);
    painter->setBrush(portColor);
    painter->drawPolygon(diamond);

    painter->setPen(QPen(style ? style->getBackgroundAltColor() : QColor(60, 60, 60), 1.2));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(diamond);
}
