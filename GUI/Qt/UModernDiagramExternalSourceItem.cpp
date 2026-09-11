#include "UModernDiagramExternalSourceItem.h"

#include "UModernDiagramNodeItem.h"
#include "UModernDiagramWidget.h"
#include "UStyleManager.h"

#include <QPainter>
#include <QFontMetricsF>
#include <QGraphicsSceneMouseEvent>
#include <QtGlobal>
#include <algorithm>
#include <climits>

UModernDiagramExternalSourceItem::UModernDiagramExternalSourceItem(UModernDiagramWidget* owner,
                                                                   const QString& sourceItemId,
                                                                   const QString& sourceItemName)
    : m_owner(owner)
    , m_sourceItemId(sourceItemId)
    , m_sourceItemName(sourceItemName)
{
    setZValue(0);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setCursor(Qt::SizeAllCursor);
    refreshDisplayLabel();
    prepareGeometryChange();
}

QString UModernDiagramExternalSourceItem::outputFullId() const
{
    return UModernDiagramWidget::externalSourceFullPathLabel(m_sourceItemId, m_sourceItemName);
}

void UModernDiagramExternalSourceItem::refreshDisplayLabel()
{
    m_displayLabel = UModernDiagramWidget::formatExternalSourceDisplayLabel(
        m_sourceItemId, m_sourceItemName, static_cast<int>(kLabelMaxWidth));
    setToolTip(UModernDiagramWidget::externalSourceFullPathLabel(m_sourceItemId, m_sourceItemName));
    m_cachedBounds = QRectF();
    prepareGeometryChange();
    update();
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
                                                     const QList<ExternalLayoutTarget>& targets,
                                                     qreal stackOffsetY)
{
    if(nodesBounds.isEmpty())
    {
        if(!targets.isEmpty() && targets.first().dstNode)
            layoutBeside(targets.first().dstNode, static_cast<int>(stackOffsetY / kStackStep));
        return;
    }

    const QRectF itemBounds = boundingRect();
    const qreal width = itemBounds.width();
    const qreal height = itemBounds.height();

    QList<qreal> candidateLeftXs;
    candidateLeftXs.append(nodesBounds.left() - kGapFromNode - width);

    qreal minTargetLeft = nodesBounds.left();
    bool anyTarget = false;
    for(const ExternalLayoutTarget& target : targets)
    {
        if(!target.dstNode)
            continue;
        const qreal tl = target.dstNode->sceneBoundingRect().left();
        minTargetLeft = anyTarget ? qMin(minTargetLeft, tl) : tl;
        anyTarget = true;
    }
    if(anyTarget)
    {
        const qreal leftOfTargets = minTargetLeft - kGapFromNode - width;
        if(qAbs(leftOfTargets - candidateLeftXs.first()) > 1.0)
            candidateLeftXs.append(leftOfTargets);
    }

    QList<qreal> candidateYs;
    candidateYs.reserve(targets.size() + 28);
    qreal targetSumY = 0.0;
    int targetCount = 0;
    QList<qreal> targetCenterYs;
    for(const ExternalLayoutTarget& target : targets)
    {
        if(!target.dstNode)
            continue;
        const qreal centerY = target.dstNode->sceneBoundingRect().center().y();
        candidateYs.append(centerY);
        targetCenterYs.append(centerY);
        targetSumY += centerY;
        ++targetCount;
    }
    const qreal avgTargetY = targetCount > 0 ? targetSumY / targetCount : nodesBounds.center().y();
    if(targetCenterYs.size() >= 2)
    {
        std::sort(targetCenterYs.begin(), targetCenterYs.end());
        candidateYs.append(targetCenterYs[targetCenterYs.size() / 2]); // median
        // Span center — good shared bus for a tall column of synapses
        candidateYs.append(0.5 * (targetCenterYs.first() + targetCenterYs.last()));
    }
    for(int i = 0; i <= 15; ++i)
        candidateYs.append(nodesBounds.top() + nodesBounds.height() * i / 15.0);

    // Outside-row candidates (still useful when the port is NOT in the left pocket)
    const qreal gap = kGapFromNode;
    candidateYs.append(nodesBounds.top() - gap);
    candidateYs.append(nodesBounds.top() - gap * 2.0);
    candidateYs.append(nodesBounds.top() - gap * 3.0);
    candidateYs.append(nodesBounds.bottom() + gap);
    candidateYs.append(nodesBounds.bottom() + gap * 2.0);
    candidateYs.append(nodesBounds.bottom() + gap * 3.0);

    qreal bestLeftX = candidateLeftXs.first();
    qreal bestCenterY = avgTargetY + stackOffsetY;
    int bestScore = INT_MAX;
    for(qreal leftX : candidateLeftXs)
    {
        for(qreal centerY : candidateYs)
        {
            centerY += stackOffsetY;
            const QRectF portRect(leftX, centerY - height / 2.0, width, height);
            // Match live scenePortPos(): right edge of item, vertical center
            const QPointF portCenter(portRect.right(), portRect.center().y());
            const int score = ExternalLinkLayout::layoutScore(
                portCenter, targets, obstacleRects, portRect, nodesBounds);
            if(score < bestScore)
            {
                bestScore = score;
                bestLeftX = leftX;
                bestCenterY = centerY;
            }
        }
    }

    setPos(bestLeftX, bestCenterY - height / 2.0);
}

QRectF UModernDiagramExternalSourceItem::boundingRect() const
{
    if(!m_cachedBounds.isNull())
        return m_cachedBounds;

    QFont font;
    font.setPointSizeF(8.0);
    QFontMetricsF fm(font);
    const QString label = fm.elidedText(m_displayLabel, Qt::ElideMiddle, kLabelMaxWidth);
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
    const QString label = fm.elidedText(m_displayLabel, Qt::ElideMiddle, kLabelMaxWidth);
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

QVariant UModernDiagramExternalSourceItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionHasChanged && scene() && m_owner)
        m_owner->updateExternalSourceLinkGeometry(this);
    return QGraphicsItem::itemChange(change, value);
}

void UModernDiagramExternalSourceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(m_owner)
        m_owner->onExternalSourceMoved(this, true);
}
