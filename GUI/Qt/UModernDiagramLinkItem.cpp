#include "UModernDiagramLinkItem.h"
#include "UModernDiagramWidget.h" // Для доступа к Port, PortCategory и UModernDiagramWidget
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramTooltipGenerator.h"
#include "UStyleManager.h"

#include <QPainterPath>
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
{
    UStyleManager* style = UStyleManager::instance();
    setPen(QPen(style->getLinkTempColor(), style->getLinkWidth(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    // Temporary links don't need hover events
    setAcceptHoverEvents(false);
    // Временные связи не кэшируем, так как они часто меняются
    updateGeometry(tempEnd);
}

void UModernDiagramLinkItem::updateGeometry(const QPointF& cursorOverride)
{
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
    }

    QPainterPath path(start);
    QPointF c1 = start + QPointF((end.x() - start.x()) * 0.4, 0);
    QPointF c2 = end   - QPointF((end.x() - start.x()) * 0.4, 0);
    path.cubicTo(c1, c2, end);
    setPath(path);
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

