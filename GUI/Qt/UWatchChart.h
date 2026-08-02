#ifndef UWATCHCHART_H
#define UWATCHCHART_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include <QWheelEvent>
#include <QKeyEvent>

#include <QString>
#include <QVector>

#include "NmsdkQtCompat.h"
#include <QtCharts/QValueAxis>
#include "UWatchSerie.h"
#include "UWatchChartView.h"
#include "Plot/PlotDocument.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

class QToolBar;
class QAction;
class PlotSettingsSidePanel;

namespace Ui {
class UWatchChart;
}
NMSDK_QT_CHARTS_USE_NS

class UWatchTab;
//////////////////////////////////////////////////////////////////////////////
// Один отдельно взятый график с одной или несколькими сериями данных
//////////////////////////////////////////////////////////////////////////////

class UWatchChart : public QWidget
{
    Q_OBJECT

public:
    explicit UWatchChart(QWidget *parent = nullptr);
    ~UWatchChart();


    //get'ы
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



    //set'ы
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

    // Функция для высталвения диапазона времени для считывания и отображения данных
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

    // Методы для батчинга обновлений
    void requestUpdate();
    void commitUpdate();

    //действия с сериями
    void createSerie(int channelIndex, const QString componentName, const QString propertyName,
                     const QString type, int jx, int jy, double time_interval, double y_shift);
    /// XY series: X and Y from property roles (same-tick pairs).
    void createSerieXY(int channelIndex,
                       const QString& xComponent, const QString& xProperty, int xJx, int xJy,
                       const QString& yComponent, const QString& yProperty, int yJx, int yJy,
                       double y_shift, NMSDK::Plot::VizKind viz = NMSDK::Plot::VizKind::XYLine);
    void deleteSerie(int serieIndex);
    void addDataToSerie(int serieIndex, double x, double y);
    int  countSeries();

    NMSDK::Plot::VizKind getVizKind() const { return vizKind; }
    void setVizKind(NMSDK::Plot::VizKind kind);

    bool isLegendVisible() const;
    void setLegendVisible(bool visible);
    bool isTitleVisible() const;
    void setTitleVisible(bool visible);

    void setInteractionTrackLatest(bool track);
    void setInteractionPan(bool pan);
    void resetViewport();

    void connectSerieTooltip(UWatchSerie* serie);

    NMSDK::Plot::PlotPanel toPlotPanel() const;
    void applyPlotPanelMeta(const NMSDK::Plot::PlotPanel& panel);

    //работа с динамикой осей
    int axisXrange;
    bool isAxisXtrackable = true;   //будет ли "поле зрения" бежать за временем
    bool isAxisYzoomable = true;    //зум по оси У (ctrl+крокрутка)
    bool isAxisYscrollable = true;  //скролл оси У

    //доступные цвета для серий (из UStyleManager)
    QColor getDefaultColor(int index) const;
    
    // Применение стилей темы к графику
    void applyTheme();


private:
    Ui::UWatchChart *ui;

    //все график, оси, скороллбар и их расположение
    QVBoxLayout *verticalLayout;
    QToolBar *modeBar = nullptr;
    QAction *actPan = nullptr;
    QAction *actBoxZoom = nullptr;
    QAction *actTrack = nullptr;
    QAction *actReset = nullptr;

    QPoint m_lastPoint;
    bool m_isPress;
    bool m_alreadySaveRange;
    double m_xMin, m_xMax, m_yMin, m_yMax;

    // parent UWatchTab
    UWatchTab* WatchTab;

    // array with initital values for axes
    // {x_range, y_max, y_min}
    std::vector<double> InitialAxesState;

    NMSDK::Plot::VizKind vizKind = NMSDK::Plot::VizKind::TimeSeries;
    bool m_legendVisible = true;
    bool m_titleVisible = true;

 public:
    UWatchChartView *chartView;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;


    //серии + данные об источнике данных
    QVector <UWatchSerie*> series;

    bool isCtrlPressed = false;
    int chartIndex; //что бы график знал какой он по счету в векторе графиков

    // Флаг для батчинга обновлений
    bool pendingUpdate = false;



private slots:
    //скролл и зум по оси Y
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
    void onModePan();
    void onModeBoxZoom();
    void onModeTrack();
    void onModeReset();
signals:
    void addSerieSignal(int someIndex);
    void UpdateTabGuiSignal(bool force_update);
    void openSettingsPanel(int chartIndex, bool seriesPage);
};

#endif // UWATCHCHART_H
