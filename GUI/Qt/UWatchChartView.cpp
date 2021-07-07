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

        double x_range = finish.x() - start.x();

        double y_min = finish.y();
        double y_max = start.y();

        rubberBand->hide();

        emit updateChartAxes(x_range, y_min, y_max);
    }
}


