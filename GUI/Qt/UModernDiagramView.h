#ifndef UMODERNDIAGRAMVIEW_H
#define UMODERNDIAGRAMVIEW_H

#include <QGraphicsView>
#include <QPoint>
#include <QPointF>

class UModernDiagramWidget;
class UModernDiagramScene;

/// Вид для современной диаграммы
/// Обрабатывает события мыши, прокрутку и drag&drop компонентов
class UModernDiagramView : public QGraphicsView
{
public:
    UModernDiagramView(UModernDiagramWidget* owner, QGraphicsScene* scene);

    // Переопределяем ensureVisible для блокировки автоматической прокрутки при выделении
    void ensureVisible(const QRectF& rect, int xmargin = 50, int ymargin = 50);

    // Переопределяем centerOn для блокировки автоматического центрирования при выделении
    void centerOn(const QPointF& pos);
    void centerOn(const QGraphicsItem* item);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool viewportEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    UModernDiagramWidget* m_owner;
    bool m_isPanning = false;
    QPoint m_lastPanPoint;
    QPoint m_rubberBandStartViewPos;  // Начальная позиция RubberBand в координатах view
    bool m_isRubberBandDragging = false;  // Флаг активного RubberBand drag
};

#endif // UMODERNDIAGRAMVIEW_H

