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

    /// When false (Pan mode), do not capture ROI / rubber-band zoom.
    void setRoiCaptureEnabled(bool enabled);
    bool isRoiCaptureEnabled() const { return m_roiEnabled; }

private:
    QRubberBand* rubberBand;
    QPoint origin;
    bool m_roiEnabled = true;

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
