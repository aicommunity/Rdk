#ifndef UWATCHSERIE_H
#define UWATCHSERIE_H

#include "NmsdkQtCompat.h"
#include "Plot/PlotDocument.h"
#include <QtCharts/QLineSeries>
#include <QPointF>
#include <QString>
#include <QVector>

namespace RDK
{
struct UControllerDataReader;
}

class UWatchSerie: public NMSDK_QT_CHARTS_BASE(QLineSeries)
{
public:
    UWatchSerie();

    // Y (and TimeSeries) source
    int indexChannel = 0;
    QString nameComponent;
    QString nameProperty;
    double YShift = 0.0;
    RDK::UControllerDataReader * data_reader = nullptr;
    int Jx = -1;
    int Jy = -1;

    // X source for XY viz (Property role)
    QString xNameComponent;
    QString xNameProperty;
    int xJx = -1;
    int xJy = -1;
    RDK::UControllerDataReader * x_data_reader = nullptr;

    NMSDK::Plot::VizKind vizKind = NMSDK::Plot::VizKind::TimeSeries;
    int windowSize = 10000;
    int xyMinIntervalMs = 0;
    double xyMinDistance = 0.0;

    // XY ring buffer runtime state (gate on reader sim-time, not FIFO size)
    QVector<QPointF> xyRing;
    double xyLastXSimTime = -1.0;
    double xyLastYSimTime = -1.0;
    double xyLastAcceptSimTime = -1.0;

    bool isOnline = true;

    void setOnlineStatus(bool online);

    NMSDK::Plot::DataBinding toBinding() const;
    void applyBinding(const NMSDK::Plot::DataBinding& binding, NMSDK::Plot::VizKind viz);
    NMSDK::Plot::PlotSeries toPlotSeries() const;
};

#endif // UWATCHSERIE_H
