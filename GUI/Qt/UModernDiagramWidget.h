#ifndef UMODERNDIAGRAMWIDGET_H
#define UMODERNDIAGRAMWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QPointer>
#include <QPoint>
#include <QTreeWidget>
#include <rdk_init.h>
#include <rdk_application.h>
#include "../Core/Engine/UXMLEnvSerialize.h"
#include "../Core/Engine/UEnvSupport.h"

/// Простой современный виджет диаграммы на основе QGraphicsView/QGraphicsScene.
/// Отображает компоненты как узлы с портами, поддерживает drag&drop связей и миникарту.
class UModernDiagramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UModernDiagramWidget(QWidget *parent = nullptr);
    ~UModernDiagramWidget() override;

    /// Устанавливает приложение и имя компонента (модели) для отображения
    void SetApplication(RDK::UApplication* app);
    void SetComponentName(const QString& name);

public slots:
    /// Перезагрузка диаграммы из текущей модели
    void Reload();
    /// Установить масштаб по содержимому
    void FitToView();
    /// Обработка двойного клика из списка компонентов
    void componentDoubleClick(QString name);
    /// Обработка одиночного клика из списка компонентов
    void componentSingleClick(QString name);
    /// Обновление схемы
    void updateScheme(bool reloadXml);
    /// Выбор компонента по имени
    void selectComponent(QString name);

signals:
    /// Компонент выбран (одиночный клик)
    void componentSelected(QString name);
    /// Двойной клик по компоненту (сигнал для внешних обработчиков)
    void componentDoubleClicked(QString name);
    /// Выход на уровень выше
    void componentStapBack();
    /// Обновление списка компонентов
    void updateComponentsList();
    /// Просмотр/разрыв связи
    void viewLinks(QString componentName);
    /// Создание связи между двумя компонентами
    void createLinks(QString firstComponentName, QString secondComponentName);
    /// Переключение связи между двумя компонентами
    void switchLinks(QString firstComponentName, QString secondComponentName);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    friend class ModernScene;
    friend class ModernGraphicsView;
    
    /// Категория порта для категоризации выходных портов
    enum class PortCategory {
        Own,        // Собственные свойства компонента
        Child,      // Свойства дочерних компонентов
        Alias       // Алиасы свойств
    };
    
    struct Port
    {
        QPointF pos;
        bool isInput;
        QString name;           // Имя порта (например, "Output", "ChannelInputs")
        QString fullPath;       // Полный путь для вложенных портов (например, "SubComp.Output")
        QString componentName;  // Имя компонента-владельца
        QString displayName;    // Отображаемое имя (для tooltip)
        PortCategory category;  // Категория порта (для выходных портов)
        
        Port() : isInput(false), category(PortCategory::Own) {}
        Port(const QPointF& p, bool input, const QString& n) 
            : pos(p), isInput(input), name(n), componentName(n), displayName(n), category(PortCategory::Own) {}
    };

    class NodeItem : public QGraphicsRectItem
    {
    public:
        NodeItem(class UModernDiagramWidget* owner, const QString& name, const QString& cls);
        ~NodeItem() override;
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
        QPointF scenePortPos(bool output) const;
        const Port* getPortAtPosition(const QPointF& localPos) const;
        // Принудительно обновляет hover-состояние по координате сцены (для периодического опроса)
        void refreshHoverAtScenePos(const QPointF& scenePos);
        QVector<Port> getNestedPorts(bool isInput, bool includeNested) const;
        QVector<Port> inputs;
        QVector<Port> outputs;
        QString nodeName;
        QString className;
    public:
        // Доступ к членам для UModernDiagramWidget::clearScene() и ModernScene
        UModernDiagramWidget* m_owner;
        mutable const Port* m_hoveredPort;
        QGraphicsProxyWidget* m_portListWidgetProxy;
        QTreeWidget* m_portListWidget;
        QTimer* m_hideTimer;
        
        friend class ModernScene;
    public:
        void showPortListWidget(const QPointF& scenePos);
        void hidePortListWidget();
        void updatePortListWidget(bool isInput, bool includeNested);
        void onPortItemActivated(QTreeWidgetItem* item, int column);
        
        // Методы для получения портов по категориям
        QVector<Port> getOwnOutputPorts() const;
        QVector<Port> getChildOutputPorts() const;
        QVector<Port> getAliasOutputPorts() const;
    };

    class LinkItem : public QGraphicsPathItem
    {
    public:
        // Финальная линия между узлами
        LinkItem(class NodeItem* src, class NodeItem* dst, bool useOutput=true, bool useInput=true);
        // Временная линия до курсора
        LinkItem(class NodeItem* src, const QPointF& tempEnd, const QPointF& startPos = QPointF());
        void updateGeometry(const QPointF& cursorOverride = QPointF());
    private:
        class NodeItem* m_src;
        class NodeItem* m_dst;
        bool m_useOutput;
        bool m_useInput;
        bool m_isTemp;
        QPointF m_tempEnd;
        QPointF m_startPos; // Начальная позиция для временной линии
    };

    void buildScene();
    void clearScene();
    void layoutGrid();
    NodeItem* pickPort(const QPointF& scenePos, bool requireInput, QPointF& portPos);
    const Port* pickPortDetailed(const QPointF& scenePos, bool requireInput, NodeItem*& node, QPointF& portPos);
    NodeItem* pickNode(const QPointF& scenePos) const;
    void buildLinks();

    // UI
    QGraphicsScene* m_scene;
    QGraphicsView*  m_mainView;
    QGraphicsView*  m_miniMap;

    // Drag state (for port-to-port drag & drop)
    LinkItem* m_tempLink;
    NodeItem* m_dragSourceNode;
    const Port* m_dragSourcePort;
    QPointF   m_dragSourcePortPos;
    
    // Active connection state (for tree widget selection)
    NodeItem* m_activeSourceNode;
    const Port* m_activeSourcePort;
    QPointF m_activeSourcePortPos;
    LinkItem* m_activeTempLink;
    bool m_isLineFrozen;  // Флаг, указывающий, что соединение "заморожено" на порту
    QPointF m_frozenTargetPortPos;  // Позиция порта, к которому "прилипло" соединение

    // Data
    RDK::UApplication* m_application;
    QString m_componentName;
    QList<NodeItem*> m_nodes;
    QHash<QString, NodeItem*> m_nodeByName;
    QList<LinkItem*> m_links;

    // Coord scaling (scene units per kernel unit)
    // Set to 30 to match UDrawEngine's ZoomCoeff for 1:1 scale with classic diagram
    double m_coordScale = 30.0;
    
    // Минимальная позиция, использованная для нормализации при загрузке
    // Нужна для правильной денормализации координат при сохранении
    QPointF m_normalizationOffset;

    // Context menu
    QMenu* m_contextMenu;
    QAction* m_actionViewOrBreakLink;
    QAction* m_actionCreateLink;
    QAction* m_actionFinishLink;
    QAction* m_actionCancelLink;
    QAction* m_actionStartMoving;
    QAction* m_actionFinishMoving;
    QAction* m_actionCancelMoving;
    QAction* m_actionSwitchLink;
    QAction* m_actionFinishSwitching;
    QAction* m_actionCancelSwitching;
    QAction* m_actionCloneComponent;
    QAction* m_actionQuickLink;
    
    // Context menu state
    QString m_firstComponentToConnection;
    QString m_startMoveComponent;
    QString m_startSwitchComponent;
    NodeItem* m_contextMenuNode;

    void createContextMenu();
    QString getSelectedComponentLongName() const;
    
    // Context menu slots
private slots:
    void componentViewOrBreakLink();
    void componentCreateLink();
    void componentFinishLink();
    void componentCancelLink();
    void componentStartMoving();
    void componentFinishMoving();
    void componentCancelMoving();
    void componentStartSwitching();
    void componentFinishSwitching();
    void componentCancelSwitching();
    void componentRename();
    void actionClassDescriptionTriggered();
    void componentDelete();
    void componentCopyNameToClipboard();
    void componentCopyLongNameToClipboard();
    void componentCopyClassNameToClipboard();
    void componentReset();
    void componentCalculate();
    void componentDefault();
    void componentGUI();
    void componentCopyXMLDescription();
    void componentCloneComponent();
    void componentQuickLink();

    QPointF scenePosFromKernel(const QPointF& kernel) const;
    QPointF kernelPosFromScene(const QPointF& scene) const;
    bool loadCoord(const QString& fullName, QPointF& outPos) const;
    void saveCoord(const QString& fullName, const QPointF& scenePos) const;
    QPointF currentMinScenePos() const;
};

#endif // UMODERNDIAGRAMWIDGET_H

