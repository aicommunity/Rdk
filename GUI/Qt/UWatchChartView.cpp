#include <QVBoxLayout>
#include <iostream>

#include "UWatchChartView.h"

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
}

void UWatchChartView::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void UWatchChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPointF start, finish;
        start = chart()->mapToValue(origin);
        finish = chart()->mapToValue(event->pos());

        double x_range = std::abs(start.x() - finish.x());

        double y_min = start.y();
        double y_max = finish.y();

        if(y_min > y_max)
            std::swap(y_min, y_max);

        rubberBand->hide();

        emit updateChartAxes(x_range, y_min, y_max);
    }
}


