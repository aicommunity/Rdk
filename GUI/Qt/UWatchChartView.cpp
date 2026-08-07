#include <QVBoxLayout>
#include <iostream>
#include <QMouseEvent>
#include <QtCharts/QChart>

#include "UWatchChartView.h"

namespace {
constexpr int kClickSlopPx = 6;
}

UWatchChartView::UWatchChartView(QWidget *parent) :
     QChartView(parent),
     rubberBand(nullptr)
{
     // Custom ROI only — Qt RectangleRubberBand would double-zoom on release.
     setRubberBand(QChartView::NoRubberBand);
     rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

UWatchChartView::~UWatchChartView()
{

}

void UWatchChartView::setRoiCaptureEnabled(bool enabled)
{
    m_roiEnabled = enabled;
    if (!enabled && rubberBand)
        rubberBand->hide();
}

QPointF UWatchChartView::mapPixelToSeriesValue(const QPoint& viewPos) const
{
    if (!chart())
        return {};
    const QPointF scenePos = mapToScene(viewPos);
    const QPointF chartPos = chart()->mapFromScene(scenePos);
    return chart()->mapToValue(chartPos);
}

void UWatchChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && m_roiEnabled)
    {
        origin = event->pos();
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
        // Do not forward to QChartView — avoids Qt rubber-band / zoom state.
        return;
    }
    QChartView::mousePressEvent(event);
}

void UWatchChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_roiEnabled && rubberBand && rubberBand->isVisible())
    {
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
        return;
    }
    QChartView::mouseMoveEvent(event);
}

void UWatchChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
    {
        QChartView::mouseReleaseEvent(event);
        return;
    }

    const QPoint delta = event->pos() - origin;
    const bool isClick = delta.manhattanLength() < kClickSlopPx;
    if (rubberBand)
        rubberBand->hide();

    if (!m_roiEnabled)
    {
        if (isClick)
            emit chartClicked();
        QChartView::mouseReleaseEvent(event);
        return;
    }

    // ROI mode: never call QChartView::mouseReleaseEvent (would zoomIn again).
    if (isClick)
    {
        emit chartClicked();
        return;
    }

    const QPointF start = mapPixelToSeriesValue(origin);
    const QPointF finish = mapPixelToSeriesValue(event->pos());

    const double x_min = start.x();
    const double x_max = finish.x();
    // Screen Y grows down; chart Y grows up — swap for ROI.
    const double y_min = finish.y();
    const double y_max = start.y();

    emit updateChartAxes(x_min, x_max, y_min, y_max);
}

void UWatchChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit chartDoubleClicked();
    QChartView::mouseDoubleClickEvent(event);
}
