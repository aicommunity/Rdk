#ifndef UMODERNDIAGRAMLINKITEM_H
#define UMODERNDIAGRAMLINKITEM_H

#include <QGraphicsPathItem>
#include <QPointF>
#include <QGraphicsSceneHoverEvent>

// Forward declarations
// ВАЖНО: Не включаем UModernDiagramWidget.h здесь в начале, чтобы избежать циклических зависимостей
// Полное определение включается в конце файла после всех forward declarations
class UModernDiagramWidget;
class UModernDiagramNodeItem;

// PortCategory теперь в отдельном файле
#include "UModernDiagramPort.h"

/// Элемент связи между узлами диаграммы
/// Отрисовывает кривую Безье между портами узлов
class UModernDiagramLinkItem : public QGraphicsPathItem
{
public:
    // Финальная линия между узлами
    UModernDiagramLinkItem(UModernDiagramNodeItem* src, UModernDiagramNodeItem* dst, bool useOutput=true, bool useInput=true);
    // Финальная линия с указанием категорий портов
    UModernDiagramLinkItem(UModernDiagramNodeItem* src, UModernDiagramNodeItem* dst,
                           PortCategory srcCategory,
                           PortCategory dstCategory);
    // Временная линия до курсора
    UModernDiagramLinkItem(UModernDiagramNodeItem* src, const QPointF& tempEnd, const QPointF& startPos = QPointF());

    void updateGeometry(const QPointF& cursorOverride = QPointF());

    // Hover events for tooltips
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // Getters for tooltip generation
    UModernDiagramNodeItem* getSourceNode() const { return m_src; }
    UModernDiagramNodeItem* getDestinationNode() const { return m_dst; }

    // Геттеры для доступа к данным связи (для оптимизации проверки соединений)
    UModernDiagramNodeItem* src() const { return m_src; }
    UModernDiagramNodeItem* dst() const { return m_dst; }
    bool hasCategories() const { return m_hasCategories; }
    PortCategory srcCategory() const { return m_srcCategory; }
    PortCategory dstCategory() const { return m_dstCategory; }
    bool useOutput() const { return m_useOutput; }
    bool useInput() const { return m_useInput; }

private:
    UModernDiagramWidget* m_owner;
    UModernDiagramNodeItem* m_src;
    UModernDiagramNodeItem* m_dst;
    bool m_useOutput;
    bool m_useInput;
    bool m_isTemp;
    QPointF m_tempEnd;
    QPointF m_startPos; // Начальная позиция для временной линии
    PortCategory m_srcCategory;  // Категория исходного порта (если известна)
    PortCategory m_dstCategory;   // Категория целевого порта (если известна)
    bool m_hasCategories;        // Флаг, указывающий, что категории заданы
};

// Включаем полное определение UModernDiagramWidget после объявления класса
// Это необходимо для доступа к вложенным типам NodeItem и PortCategory
// ВАЖНО: UModernDiagramWidget.h больше не включает UModernDiagramLinkItem.h,
// что предотвращает циклические зависимости
#include "UModernDiagramWidget.h"

#endif // UMODERNDIAGRAMLINKITEM_H

