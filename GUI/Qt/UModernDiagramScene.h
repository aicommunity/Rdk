#ifndef UMODERNDIAGRAMSCENE_H
#define UMODERNDIAGRAMSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QPointF>

class UModernDiagramWidget;

/// Сцена для современной диаграммы
/// Обрабатывает события мыши и управляет взаимодействием с узлами
class UModernDiagramScene : public QGraphicsScene
{
public:
    explicit UModernDiagramScene(UModernDiagramWidget* owner);
    ~UModernDiagramScene() override = default;

    /// Публичный метод для проверки, идет ли перемещение группы
    bool isGroupMoving() const { return m_isGroupMoving; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void pollHover();

    UModernDiagramWidget* m_owner;
    QTimer* m_hoverTimer{nullptr};
    QPointF m_rubberBandStartPos;  // Начальная позиция для RubberBandDrag
    bool m_isRubberBandActive;  // Флаг активного RubberBandDrag
    bool m_isGroupSelected = false;  // Есть ли выделенная группа объектов
    bool m_isGroupMoving = false;    // Идет ли перемещение группы
    QList<class UModernDiagramNodeItem*> m_savedSelection; // Сохраненное выделение для перемещения
    QPointF m_lastHoverPos;  // Последняя позиция курсора для оптимизации
    bool m_isProcessingMouseEvent = false;  // Флаг обработки событий мыши для пропуска pollHover
};

#endif // UMODERNDIAGRAMSCENE_H

