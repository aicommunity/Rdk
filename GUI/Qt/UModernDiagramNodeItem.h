#ifndef UMODERNDIAGRAMNODEITEM_H
#define UMODERNDIAGRAMNODEITEM_H

#include <QGraphicsRectItem>
#include <QPointF>
#include <QVector>
#include <QMap>
#include <QList>
#include <QTreeWidget>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QGraphicsSceneHoverEvent>
#include <memory>

// Forward declarations
class UModernDiagramWidget;
class UModernDiagramLinkItem;

// ВАЖНО: Не включаем UModernDiagramWidget.h здесь, чтобы избежать циклических зависимостей
// Полное определение включается только в .cpp файле
// Port и PortCategory теперь в отдельном файле UModernDiagramPort.h
#include "UModernDiagramPort.h"

/// Элемент узла диаграммы
/// Представляет компонент на диаграмме с портами входа и выхода
class UModernDiagramNodeItem : public QGraphicsRectItem
{
public:
    UModernDiagramNodeItem(UModernDiagramWidget* owner, const QString& name, const QString& cls);
    ~UModernDiagramNodeItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    QPointF scenePortPos(bool output) const;
    QPointF scenePortPosByCategory(bool output, UModernDiagramPortCategory category) const;
    UModernDiagramPortCategory determinePortCategory(const QString& propertyName, bool isInput) const;
    const UModernDiagramPort* getPortAtPosition(const QPointF& localPos) const;
    // Принудительно обновляет hover-состояние по координате сцены (для периодического опроса)
    void refreshHoverAtScenePos(const QPointF& scenePos);
    QVector<UModernDiagramPort> getNestedPorts(bool isInput, bool includeNested) const;

    // Публичные данные
    QVector<Port> inputs;
    QVector<Port> outputs;
    QString nodeName;
    QString className;

    // Доступ к членам для UModernDiagramWidget::clearScene() и UModernDiagramScene
    UModernDiagramWidget* m_owner;
    mutable const Port* m_hoveredPort;
    QGraphicsProxyWidget* m_portListWidgetProxy;
    QTreeWidget* m_portListWidget;
    std::unique_ptr<QTimer> m_hideTimer;
    // Кэш связей, подключенных к этому узлу (для оптимизации обновления при перемещении)
    QList<UModernDiagramLinkItem*> m_connectedLinks;
    // Кэш для результатов дорогих вычислений в paint()
    mutable bool m_cacheValid;
    // Используем QMap вместо QHash, так как для QMap не нужна функция qHash
    mutable QMap<UModernDiagramPortCategory, bool> m_hasConnectionsToInputCache;
    mutable QMap<UModernDiagramPortCategory, bool> m_hasConnectionsToOutputCache;
    mutable QMap<UModernDiagramPortCategory, bool> m_hasInputPortsCache;
    mutable QMap<UModernDiagramPortCategory, bool> m_hasOutputPortsCache;
    // Кэш для самих портов (ленивая загрузка)
    mutable bool m_portsCacheValid = false;
    mutable QVector<Port> m_cachedOwnInputPorts;
    mutable QVector<Port> m_cachedChildInputPorts;
    mutable QVector<Port> m_cachedAliasInputPorts;
    mutable QVector<Port> m_cachedOwnOutputPorts;
    mutable QVector<Port> m_cachedChildOutputPorts;
    mutable QVector<Port> m_cachedAliasOutputPorts;
    // Последняя позиция курсора для throttling в hoverMoveEvent
    QPointF m_lastHoverMovePos;

    friend class UModernDiagramScene;

public:
    void showPortListWidget(const QPointF& scenePos);
    void hidePortListWidget();
    void updatePortListWidget(bool isInput, bool includeNested);
    void onPortItemActivated(QTreeWidgetItem* item, int column);

    // Методы для получения портов по категориям
    QVector<Port> getOwnOutputPorts() const;
    QVector<Port> getChildOutputPorts() const;
    QVector<Port> getAliasOutputPorts() const;
    QVector<Port> getOwnInputPorts() const;
    QVector<Port> getChildInputPorts() const;
    QVector<Port> getAliasInputPorts() const;

    // Методы для проверки наличия соединений к портам категории
    bool hasConnectionsToInputCategory(UModernDiagramPortCategory category) const;
    bool hasConnectionsToOutputCategory(UModernDiagramPortCategory category) const;
};

#endif // UMODERNDIAGRAMNODEITEM_H

