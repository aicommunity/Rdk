#ifndef UWATCHCHARTVIEW_H
#define UWATCHCHARTVIEW_H


#include <QChartView>
#include <QRubberBand>


using namespace QtCharts;


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

private:
signals:
    void updateChartAxes(double x_min, double x_max, double y_min, double y_max);

};

#endif // UWATCHCHARTVIEW_H
