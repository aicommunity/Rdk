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

signals:
    void updateChartAxes(double x_min, double x_max, double y_min, double y_max);
    /// Left-click without meaningful drag (focus chart without zoom).
    void chartClicked();
    void chartDoubleClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QRubberBand* rubberBand;
    QPoint origin;
    bool m_roiEnabled = true;
};

#endif // UWATCHCHARTVIEW_H
