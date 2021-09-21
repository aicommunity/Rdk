#ifndef UWATCHCHART_H
#define UWATCHCHART_H

#include <QWidget>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QMenu>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include <QWheelEvent>
#include <QKeyEvent>

#include <QString>
#include <QVector>

#include <QValueAxis>
#include "UWatchSerie.h"
#include "UWatchChartView.h"
#include <QChart>
#include <QChartView>


namespace Ui {
class UWatchChart;
}
using namespace QtCharts;

class UWatchTab;
//////////////////////////////////////////////////////////////////////////////
// ���� �������� ������ ������ � ����� ��� ����������� ������� ������
//////////////////////////////////////////////////////////////////////////////

class UWatchChart : public QWidget
{
    Q_OBJECT

public:
    explicit UWatchChart(QWidget *parent = nullptr);
    ~UWatchChart();


    //get'�
    QString getChartTitle();

    QString getAxisXName();
    QString getAxisYName();
    double  getAxisXmin();
    double  getAxisXmax();
    double  getAxisYmin();
    double  getAxisYmax();
    double  getInitialAxisYmin();
    double  getInitialAxisYmax();

    UWatchSerie *getSerie(int index); //
    QString      getSerieName(int serieIndex);
    QColor       getSerieColor(int serieIndex);
    int          getSerieWidth(int serieIndex);
    Qt::PenStyle getSerieLineType(int serieIndex);
    double       getSerieYShift(int serieIndex);



    //set'�
    void setChartTitle(QString title);
    void setChartIndex(int index);

    void setAxisXname(QString name);
    void setAxisYname(QString name);
    void setAxisXmin(double value);
    void setAxisXmax(double value);
    void setAxisYmin(double value);
    void setAxisYmax(double value);
    double getAxisXrange(void) const;
    void setAxisXrange(double value);
    bool getIsAxisXtrackable(void) const;

    // ������� ��� ����������� ��������� ������� ��� ���������� � ����������� ������
    void updateTimeIntervals(double value);

    void setSerieName(int serieIndex, QString name);
    void setSerieColor(int serieIndex, int colorIndex);
    void setSerieLineType(int serieIndex, Qt::PenStyle lineType);
    void setSerieWidth(int serieIndex, int width);
    void setSerieStyle(int serieIndex, QColor color, int width, Qt::PenStyle lineType);
    void setSerieYshift(int serieIndex, int y_shift);

    void fixInitialAxesState();
    void restoreInitialAxesState();
    bool checkZoomed(void);

    //�������� � �������
    void createSerie(int channelIndex, const QString componentName, const QString propertyName,
                     const QString type, int jx, int jy, double time_interval, double y_shift);
    void deleteSerie(int serieIndex);
    void addDataToSerie(int serieIndex, double x, double y);
    int  countSeries();

    //������ � ��������� ����
    int axisXrange;
    bool isAxisXtrackable = true;   //����� �� "���� ������" ������ �� ��������
    bool isAxisYzoomable = true;    //��� �� ��� � (ctrl+���������)
    bool isAxisYscrollable = true;  //������ ��� �

    //��������� ����� ��� �����
    const QColor defaultColors[15]={Qt::red, Qt::darkRed, Qt::yellow, Qt::darkYellow, Qt::green, Qt::darkGreen, Qt::cyan, Qt::darkCyan,
                                   Qt::blue, Qt::darkBlue, Qt::magenta,Qt::darkMagenta, Qt::gray, Qt::darkGray, Qt::black};


private:
    Ui::UWatchChart *ui;

    //��� ������, ���, ���������� � �� ������������
    QVBoxLayout *verticalLayout;
    QScrollBar *horizontalScrolBar;

    QPoint m_lastPoint;
    bool m_isPress;
    bool m_alreadySaveRange;
    double m_xMin, m_xMax, m_yMin, m_yMax;
    QGraphicsSimpleTextItem* m_coordItem;

    // parent UWatchTab
    UWatchTab* WatchTab;

    // array with initital values for axes
    // {x_range, y_max, y_min}
    std::vector<double> InitialAxesState;

 public:
    UWatchChartView *chartView;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;


    //����� + ������ �� ��������� ������
    QVector <UWatchSerie*> series;

    bool isCtrlPressed = false;
    int chartIndex; //��� �� ������ ���� ����� �� �� ����� � ������� ��������



private slots:
    //������ � ��� �� ��� Y
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void slotCustomMenuRequested(QPoint pos);
    void addSeriesSlot();
    void seriesOptionSlot();
    void chartOptionSlot();
    void saveToJpegSlot();
    void restoreAxes();
    void updateAxes(double x_min, double x_max, double y_min, double y_max);
signals:
    void addSerieSignal(int someIndex);
    void UpdateTabGuiSignal(bool force_update);
};

#endif // UWATCHCHART_H
