#include "UModernDiagramExternalSinkItem.h"

#include "UModernDiagramNodeItem.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramExternalSinkLayout.h"
#include "UStyleManager.h"

#include <QPainter>
#include <QFontMetricsF>
#include <QGraphicsSceneMouseEvent>
#include <QtGlobal>

UModernDiagramExternalSinkItem::UModernDiagramExternalSinkItem(UModernDiagramWidget* owner,
                                                               const QString& outputItemId,
                                                               const QString& outputItemName)
    : m_owner(owner)
    , m_outputItemId(outputItemId)
    , m_outputItemName(outputItemName)
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

QString UModernDiagramExternalSinkItem::outputFullId() const
{
    return UModernDiagramWidget::externalSourceFullPathLabel(m_outputItemId, m_outputItemName);
}

void UModernDiagramExternalSinkItem::refreshDisplayLabel()
{
    if(m_owner)
    {
        m_displayLabel = UModernDiagramWidget::formatExternalSinkDisplayLabel(
            m_outputItemId,
            m_outputItemName,
            m_owner->m_componentName,
            m_owner->m_componentName.section(QLatin1Char('.'), -1),
            static_cast<int>(kLabelMaxWidth));
    }
    else
    {
        m_displayLabel = UModernDiagramWidget::formatExternalSourceDisplayLabel(
            m_outputItemId, m_outputItemName, static_cast<int>(kLabelMaxWidth));
    }
    setToolTip(UModernDiagramWidget::externalSourceFullPathLabel(m_outputItemId, m_outputItemName));
    m_cachedBounds = QRectF();
    prepareGeometryChange();
    update();
}

QPointF UModernDiagramExternalSinkItem::scenePortPos() const
{
    // Connection tip: left tip of the diamond (stub arrives from the local output on the left).
    return mapToScene(QPointF(boundingRect().left(), boundingRect().center().y()));
}

void UModernDiagramExternalSinkItem::layoutBeside(UModernDiagramNodeItem* srcNode, int stackIndex)
{
    if(!srcNode)
        return;
    const QRectF br = boundingRect();
    setPos(ExternalSinkLayout::sinkTopLeftBesideSource(
        srcNode->sceneBoundingRect(),
        br.size(),
        stackIndex,
        kGapFromNode,
        kStackStep));
}

void UModernDiagramExternalSinkItem::layoutOptimal(const QRectF& nodesBounds,
                                                   const QList<QRectF>& obstacleRects,
                                                   UModernDiagramNodeItem* srcNode,
                                                   PortCategory srcCategory,
                                                   qreal stackOffsetY)
{
    Q_UNUSED(nodesBounds)
    Q_UNUSED(obstacleRects)
    Q_UNUSED(srcCategory)
    // Right-side sinks stay beside their source node. A global bounds search is easy to
    // confuse with left-pocket incoming layout and is unnecessary for short cubic stubs.
    if(srcNode)
        layoutBeside(srcNode, static_cast<int>(stackOffsetY / kStackStep));
}

QRectF UModernDiagramExternalSinkItem::boundingRect() const
{
    if(!m_cachedBounds.isNull())
        return m_cachedBounds;

    QFont font;
    font.setPointSizeF(8.0);
    QFontMetricsF fm(font);
    const QString label = fm.elidedText(m_displayLabel, Qt::ElideMiddle, kLabelMaxWidth);
    const qreal labelWidth = qMin(kLabelMaxWidth, fm.horizontalAdvance(label));
    const qreal labelHeight = fm.height();
    const qreal width = kPortSize * 2.0 + kLabelGap + labelWidth;
    const qreal height = qMax(labelHeight, kPortSize * 2.0) + 4.0;
    m_cachedBounds = QRectF(0, 0, width, height);
    return m_cachedBounds;
}

void UModernDiagramExternalSinkItem::paint(QPainter* painter,
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

    // Diamond on the LEFT (faces the source); label to the RIGHT of the diamond.
    const qreal portCenterX = bounds.left() + kPortSize;
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

    const QRectF textRect(bounds.left() + kPortSize * 2.0 + kLabelGap,
                          centerY - labelHeight / 2.0,
                          labelWidth,
                          labelHeight);
    painter->setPen(style ? style->getTextSecondaryColor() : QColor(180, 180, 180));
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, label);
}

QVariant UModernDiagramExternalSinkItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionHasChanged && scene() && m_owner)
        m_owner->updateExternalSinkLinkGeometry(this);
    return QGraphicsItem::itemChange(change, value);
}

void UModernDiagramExternalSinkItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(m_owner)
        m_owner->onExternalSinkMoved(this, true);
}
