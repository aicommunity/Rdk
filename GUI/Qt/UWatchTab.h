#ifndef UWATCHTAB_H
#define UWATCHTAB_H

#include <iostream>

#include <QWidget>
#include <QString>
#include <QVector>
#include <QSplitter>

#include "UWatchChart.h"
#include "UMatrixFormDialog.h"
#include "UVisualControllerWidget.h"
#include "UComponentPropertySelectionWidget.h"
#include "Plot/PlotDocument.h"
#include "NmsdkQtCompat.h"

NMSDK_QT_CHARTS_USE_NS

namespace Ui {
class UWatchTab;
}

class UWatchChart;
class PlotSettingsSidePanel;

class UWatchTab : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UWatchTab(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatchTab();

    void createGridLayout(int rowNumber, int colNumber);

    UWatchChart *getChart(int index);
    int countGraphs();

    void createSelectionDialog(int chartIndex);
    void createXYSelectionDialog(int chartIndex);

    int UpdateIntervalMs = 200;
    void saveUpdateInterval(int newInterval);

    int getColNumber();
    int getRowNumber();

    /// Live PlotDocument mirror (synced from charts before save / after mutations).
    const NMSDK::Plot::PlotDocument& plotDocument() const { return m_document; }
    void syncDocumentFromCharts();
    NMSDK::Plot::PlotDocument capturePlotDocument() const;
    void applyPlotDocument(const NMSDK::Plot::PlotDocument& doc);

    virtual void ASaveParameters(RDK::USerStorageXML &xml);
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

    void updateTheme();

    QList<int> captureColSplitterSizes() const;
    QVector<QList<int>> captureRowSplitterSizes() const;

private:
    void createGraph();
    void deleteGraph(int index);
    void deleteGraphs(int new_graph_count);

    void createSplitterGrid(int rowNumber);
    void ensureSettingsPanel();
    void applySplitterSizes(const NMSDK::Plot::PlotDocument& doc);

    int tabColNumber=0;
    int tabRowNumber=0;

    QVector <UWatchChart*> graph;
    std::list<double> XData;
    std::list<double> YData;
    QVector<QPointF> points;

    QSplitter *colSplitter;
    QVector <QSplitter*> rowSplitter;
    QSplitter *rootSplitter = nullptr;
    PlotSettingsSidePanel *settingsPanel = nullptr;
    NMSDK::Plot::PlotDocument m_document;

    Ui::UWatchTab *ui;

    virtual void AUpdateInterface();
    virtual void AClearInterface();
    virtual void ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata);

public slots:
    void createSelectionDialogSlot(int index);
    void seriesOptionTriggered();
    void chartsOptionTriggered();
    void openSettingsPanelSlot(int chartIndex, bool seriesPage);

};

#endif // UWATCHTAB_H
