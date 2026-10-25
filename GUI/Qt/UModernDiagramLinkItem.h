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
class UModernDiagramExternalSourceItem;
class UModernDiagramExternalSinkItem;

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
    UModernDiagramLinkItem(UModernDiagramExternalSourceItem* externalSrc,
                           UModernDiagramNodeItem* dst,
                           PortCategory dstCategory,
                           const QString& srcLabelForTooltip);
    /// Stub from local node output to right-side virtual sink port.
    UModernDiagramLinkItem(UModernDiagramNodeItem* src,
                           UModernDiagramExternalSinkItem* externalSink,
                           PortCategory srcCategory,
                           const QString& sinkLabelForTooltip);
    // Временная линия до курсора
    UModernDiagramLinkItem(UModernDiagramNodeItem* src, const QPointF& tempEnd, const QPointF& startPos = QPointF());

    void updateGeometry(const QPointF& cursorOverride = QPointF());

    // Hover events for tooltips
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

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

    bool isExternalIncoming() const { return m_isExternalIncoming; }
    bool isExternalOutgoing() const { return m_isExternalOutgoing; }
    UModernDiagramExternalSourceItem* externalSource() const { return m_externalSrc; }
    UModernDiagramExternalSinkItem* externalSink() const { return m_externalSink; }
    QString externalSourceLabel() const { return m_externalSrcLabel; }
    QString externalSinkLabel() const { return m_externalSinkLabel; }

    int parallelCount() const { return m_parallelCount; }
    void setParallelCount(int count) { m_parallelCount = count > 0 ? count : 1; }
    void incrementParallelCount() { ++m_parallelCount; }

    /// Fan-out lane index for ExternalCorridor (vertical envelope offset).
    int routeParallelIndex() const { return m_routeParallelIndex; }
    void setRouteParallelIndex(int index) { m_routeParallelIndex = index >= 0 ? index : 0; }

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
    int m_parallelCount = 1;     // Число свёрнутых в одну линию connector'ов
    int m_routeParallelIndex = 0; // Lane for corridor fan-out
    UModernDiagramExternalSourceItem* m_externalSrc = nullptr;
    QString m_externalSrcLabel;
    bool m_isExternalIncoming = false;
    UModernDiagramExternalSinkItem* m_externalSink = nullptr;
    QString m_externalSinkLabel;
    bool m_isExternalOutgoing = false;
};

// Включаем полное определение UModernDiagramWidget после объявления класса
// Это необходимо для доступа к вложенным типам NodeItem и PortCategory
// ВАЖНО: UModernDiagramWidget.h больше не включает UModernDiagramLinkItem.h,
// что предотвращает циклические зависимости
#include "UModernDiagramWidget.h"

#endif // UMODERNDIAGRAMLINKITEM_H

