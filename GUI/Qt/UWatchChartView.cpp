#include <QVBoxLayout>
#include <iostream>
#include <QMouseEvent>

#include "UWatchChartView.h"

namespace {
constexpr int kClickSlopPx = 6;
}

UWatchChartView::UWatchChartView(QWidget *parent) :
     QChartView(parent),
     rubberBand(nullptr)
{
     setRubberBand(QChartView::RectangleRubberBand);
     rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

UWatchChartView::~UWatchChartView()
{

}

void UWatchChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        origin = event->pos();
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
    }
    QChartView::mousePressEvent(event);
}

void UWatchChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBand && rubberBand->isVisible())
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    QChartView::mouseMoveEvent(event);
}

void UWatchChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        const QPoint delta = event->pos() - origin;
        const bool isClick = delta.manhattanLength() < kClickSlopPx;
        rubberBand->hide();

        if (isClick)
        {
            emit chartClicked();
        }
        else
        {
            QPointF start = chart()->mapToValue(origin);
            QPointF finish = chart()->mapToValue(event->pos());

            double x_min = start.x();
            double x_max = finish.x();
            double y_min = finish.y();
            double y_max = start.y();

            emit updateChartAxes(x_min, x_max, y_min, y_max);
        }
    }
    QChartView::mouseReleaseEvent(event);
}
