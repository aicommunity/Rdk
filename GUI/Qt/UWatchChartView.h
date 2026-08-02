#ifndef UWATCHCHARTVIEW_H
#define UWATCHCHARTVIEW_H


#include "NmsdkQtCompat.h"
#include <QtCharts/QChartView>
#include <QRubberBand>

NMSDK_QT_CHARTS_USE_NS


class UWatchChartView : public QChartView
{
    Q_OBJECT

public:
    explicit UWatchChartView(QWidget *parent = nullptr);
    ~UWatchChartView();

private:
    QRubberBand* rubberBand;
    QPoint origin;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void updateChartAxes(double x_min, double x_max, double y_min, double y_max);
    /// Left-click without meaningful drag (focus chart without zoom).
    void chartClicked();

};

#endif // UWATCHCHARTVIEW_H
