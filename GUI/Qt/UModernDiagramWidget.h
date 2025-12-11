#ifndef UMODERNDIAGRAMWIDGET_H
#define UMODERNDIAGRAMWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QTimer>
#include <QPointer>
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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    friend class ModernScene;
    friend class ModernGraphicsView;
    struct Port
    {
        QPointF pos;
        bool isInput;
        QString name;
    };

    class NodeItem : public QGraphicsRectItem
    {
    public:
        NodeItem(class UModernDiagramWidget* owner, const QString& name, const QString& cls);
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
        QPointF scenePortPos(bool output) const;
        QVector<Port> inputs;
        QVector<Port> outputs;
        QString nodeName;
        QString className;
    private:
        UModernDiagramWidget* m_owner;
    };

    class LinkItem : public QGraphicsPathItem
    {
    public:
        // Финальная линия между узлами
        LinkItem(class NodeItem* src, class NodeItem* dst, bool useOutput=true, bool useInput=true);
        // Временная линия до курсора
        LinkItem(class NodeItem* src, const QPointF& tempEnd);
        void updateGeometry(const QPointF& cursorOverride = QPointF());
    private:
        class NodeItem* m_src;
        class NodeItem* m_dst;
        bool m_useOutput;
        bool m_useInput;
        bool m_isTemp;
        QPointF m_tempEnd;
    };

    void buildScene();
    void clearScene();
    void layoutGrid();
    NodeItem* pickPort(const QPointF& scenePos, bool requireInput, QPointF& portPos);
    void buildLinks();

    // UI
    QGraphicsScene* m_scene;
    QGraphicsView*  m_mainView;
    QGraphicsView*  m_miniMap;

    // Drag state
    LinkItem* m_tempLink;
    NodeItem* m_dragSourceNode;
    QPointF   m_dragSourcePort;

    // Data
    RDK::UApplication* m_application;
    QString m_componentName;
    QList<NodeItem*> m_nodes;
    QHash<QString, NodeItem*> m_nodeByName;
    QList<LinkItem*> m_links;

    // Coord scaling (scene units per kernel unit)
    double m_coordScale = 1.0;

    QPointF scenePosFromKernel(const QPointF& kernel) const;
    QPointF kernelPosFromScene(const QPointF& scene) const;
    bool loadCoord(const QString& fullName, QPointF& outPos) const;
    void saveCoord(const QString& fullName, const QPointF& scenePos) const;
    QPointF currentMinScenePos() const;
};

#endif // UMODERNDIAGRAMWIDGET_H

