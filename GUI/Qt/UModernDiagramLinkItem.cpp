#include "UModernDiagramLinkItem.h"
#include "UModernDiagramWidget.h" // Для доступа к Port, PortCategory и UModernDiagramWidget
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramExternalSourceItem.h"
#include "UModernDiagramExternalSinkItem.h"
#include "UModernDiagramLinkRouter.h"
#include "UModernDiagramTooltipGenerator.h"
#include "UStyleManager.h"

#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsSceneHoverEvent>

UModernDiagramLinkItem::UModernDiagramLinkItem(UModernDiagramNodeItem* src, UModernDiagramNodeItem* dst, bool useOutput, bool useInput)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(dst)
    , m_useOutput(useOutput)
    , m_useInput(useInput)
    , m_isTemp(false)
    , m_srcCategory(PortCategory::Own)
    , m_dstCategory(PortCategory::Own)
    , m_hasCategories(false)
    , m_parallelCount(1)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    // Включаем кэширование для оптимизации отрисовки
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    updateGeometry();
}

UModernDiagramLinkItem::UModernDiagramLinkItem(UModernDiagramNodeItem* src, UModernDiagramNodeItem* dst,
                                               PortCategory srcCategory,
                                               PortCategory dstCategory)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(dst)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(false)
    , m_srcCategory(srcCategory)
    , m_dstCategory(dstCategory)
    , m_hasCategories(true)
    , m_parallelCount(1)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    // Включаем кэширование для оптимизации отрисовки
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    // ОПТИМИЗАЦИЯ: updateGeometry() вызывается отдельно после создания всех связей в buildLinks()
    // для ускорения массового создания связей
    // updateGeometry();
}

UModernDiagramLinkItem::UModernDiagramLinkItem(UModernDiagramNodeItem* src, const QPointF& tempEnd, const QPointF& startPos)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(nullptr)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(true)
    , m_tempEnd(tempEnd)
    , m_startPos(startPos)
    , m_srcCategory(PortCategory::Own)
    , m_dstCategory(PortCategory::Own)
    , m_hasCategories(false)
    , m_parallelCount(1)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkTempColor(), style->getLinkWidth(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    // Temporary links don't need hover events
    setAcceptHoverEvents(false);
    // Временные связи не кэшируем, так как они часто меняются
    updateGeometry(tempEnd);
}

UModernDiagramLinkItem::UModernDiagramLinkItem(UModernDiagramExternalSourceItem* externalSrc,
                                               UModernDiagramNodeItem* dst,
                                               PortCategory dstCategory,
                                               const QString& srcLabelForTooltip)
    : QGraphicsPathItem()
    , m_owner(dst ? dst->m_owner : nullptr)
    , m_src(nullptr)
    , m_dst(dst)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(false)
    , m_srcCategory(PortCategory::Own)
    , m_dstCategory(dstCategory)
    , m_hasCategories(true)
    , m_parallelCount(1)
    , m_externalSrc(externalSrc)
    , m_externalSrcLabel(srcLabelForTooltip)
    , m_isExternalIncoming(true)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(),
                Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
}

UModernDiagramLinkItem::UModernDiagramLinkItem(UModernDiagramNodeItem* src,
                                               UModernDiagramExternalSinkItem* externalSink,
                                               PortCategory srcCategory,
                                               const QString& sinkLabelForTooltip)
    : QGraphicsPathItem()
    , m_owner(src ? src->m_owner : nullptr)
    , m_src(src)
    , m_dst(nullptr)
    , m_useOutput(true)
    , m_useInput(true)
    , m_isTemp(false)
    , m_srcCategory(srcCategory)
    , m_dstCategory(PortCategory::Own)
    , m_hasCategories(true)
    , m_parallelCount(1)
    , m_externalSink(externalSink)
    , m_externalSinkLabel(sinkLabelForTooltip)
    , m_isExternalOutgoing(true)
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkColor(), style->getLinkWidth(),
                Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
}

void UModernDiagramLinkItem::updateGeometry(const QPointF& cursorOverride)
{
    auto applyRoute = [this](const QPointF& start, const QPointF& end, bool externalIncoming) {
        UModernDiagramLinkRouter::RouteRequest req;
        req.start = start;
        req.end = end;
        req.externalIncoming = externalIncoming;
        req.parallelIndex = m_routeParallelIndex;
        if(m_owner)
        {
            req.obstacles = m_owner->routingObstacles(m_dst, m_src);
            req.diagramBounds = m_owner->routingNodesBounds();
        }
        // Outgoing stubs stay short cubic; incoming uses Auto→ExternalCorridor
        const UModernDiagramLinkRouter::RouteMode mode = m_isExternalOutgoing
            ? UModernDiagramLinkRouter::RouteMode::CubicFallback
            : UModernDiagramLinkRouter::RouteMode::Auto;
        const UModernDiagramLinkRouter::RouteResult result =
            UModernDiagramLinkRouter::route(req, mode);
        prepareGeometryChange();
        setPath(result.path);

        // Reverse internal links use green stroke; external/temp keep ctor pen style
        if(!externalIncoming && !m_isExternalOutgoing && !m_isTemp)
        {
            UStyleManager* style = UStyleManager::instance();
            QPen p = pen();
            const bool reverse = UModernDiagramLinkRouter::isReverseLink(start, end);
            p.setColor(reverse ? style->getLinkReverseColor() : style->getLinkColor());
            p.setStyle(Qt::SolidLine);
            setPen(p);
        }
    };

    if(m_isExternalIncoming && m_externalSrc && m_dst)
    {
        const QPointF start = m_externalSrc->scenePortPos();
        const QPointF end = m_dst->scenePortPosByCategory(false, m_dstCategory);
        applyRoute(start, end, true);
        return;
    }

    if(m_isExternalOutgoing && m_externalSink && m_src)
    {
        const QPointF start = m_src->scenePortPosByCategory(true, m_srcCategory);
        const QPointF end = m_externalSink->scenePortPos();
        applyRoute(start, end, false);
        return;
    }

    if(!m_src)
        return;

    QPointF start;
    // Для временной линии используем сохраненную позицию порта, если она задана
    if(m_isTemp && !m_startPos.isNull())
    {
        start = m_startPos;
    }
    else if(m_isTemp)
    {
        // Fallback: используем позицию выходного порта
        start = m_src->scenePortPos(false);
    }
    else
    {
        // Если категория известна, используем позицию порта этой категории
        if(m_hasCategories)
        {
            start = m_src->scenePortPosByCategory(true, m_srcCategory);
        }
        else
        {
            start = m_src->scenePortPos(true);
        }
    }

    QPointF end;
    if(m_dst)
    {
        // Если категория известна, используем позицию порта этой категории
        if(m_hasCategories)
        {
            end = m_dst->scenePortPosByCategory(false, m_dstCategory);
        }
        else
        {
            end = m_dst->scenePortPos(false);
        }
    }
    else
    {
        end = cursorOverride.isNull() ? m_tempEnd : cursorOverride;
        // Temporary rubber-band: keep lightweight cubic for responsiveness
        QPainterPath path(start);
        const qreal dx = end.x() - start.x();
        path.cubicTo(start + QPointF(dx * 0.4, 0),
                     end - QPointF(dx * 0.4, 0),
                     end);
        prepareGeometryChange();
        setPath(path);
        return;
    }

    applyRoute(start, end, false);
}

QRectF UModernDiagramLinkItem::boundingRect() const
{
    QRectF r = QGraphicsPathItem::boundingRect();
    if(m_parallelCount > 1)
        r = r.adjusted(-14, -14, 14, 14);
    return r;
}

void UModernDiagramLinkItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPathItem::paint(painter, option, widget);
    if(m_isTemp || m_parallelCount <= 1)
        return;

    const QPainterPath p = path();
    if(p.isEmpty())
        return;

    // Число только у приёмника (конец линии), фон как у ModernDiagram — перекрывает линию
    const QPointF nearEnd = p.pointAtPercent(0.88);
    const QString text = QString::number(m_parallelCount);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qMax(8.0, font.pointSizeF()));

    const QFontMetricsF fm(font);
    QRectF textRect = fm.boundingRect(text);
    textRect.moveCenter(nearEnd);
    textRect.adjust(-3, -1, 3, 1);

    painter->save();
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(UStyleManager::instance()->getBackgroundAltColor());
    painter->drawRect(textRect);
    painter->setPen(pen().color());
    painter->setBrush(Qt::NoBrush);
    painter->drawText(textRect, Qt::AlignCenter, text);
    painter->restore();
}

void UModernDiagramLinkItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPathItem::hoverEnterEvent(event);
    if(m_owner && !m_isTemp)
    {
        QString tooltip = UModernDiagramTooltipGenerator::generateLinkTooltip(this);
        setToolTip(tooltip);
    }
}

void UModernDiagramLinkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPathItem::hoverLeaveEvent(event);
    setToolTip(QString());
}
