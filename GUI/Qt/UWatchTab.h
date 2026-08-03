#ifndef UWATCHTAB_H
#define UWATCHTAB_H

#include <iostream>

#include <QWidget>
#include <QString>
#include <QVector>
#include <QSplitter>

#include "UWatchChart.h"
#include "UVisualControllerWidget.h"
#include "Plot/PlotDocument.h"
#include "Plot/PlotSettingsSidePanel.h"
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
    int activeChartIndex() const { return m_activeChartIndex; }
    void setActiveChart(int index);

    void createSelectionDialog(int chartIndex);

    int UpdateIntervalMs = 200;
    void saveUpdateInterval(int newInterval);

    int getColNumber();
    int getRowNumber();

    const NMSDK::Plot::PlotDocument& plotDocument() const { return m_document; }
    void syncDocumentFromCharts();
    NMSDK::Plot::PlotDocument capturePlotDocument() const;
    void applyPlotDocument(const NMSDK::Plot::PlotDocument& doc);

    void showInspector(PlotInspectorPage page, int chartIndex = -1);
    void hideInspector();
    bool isInspectorVisible() const;

    void toggleExpandChart(int chartIndex);
    void collapseExpandedChart();
    bool isChartExpanded() const { return m_expandedIndex >= 0; }
    int expandedChartIndex() const { return m_expandedIndex; }

    /// `<ProjectPath>/SavedWatches/` or empty if no project.
    QString savedWatchesRoot() const;
    /// Ensures session folder `<SavedWatches>/<datetime>/`; empty on failure.
    QString ensureQuickSaveSessionDir();
    bool exportChartToPath(int chartIndex, const QString& path);
    int exportAllChartsToDirectory(const QString& dirPath, const QString& extension = QStringLiteral("png"));
    int quickSaveAllCharts();
    bool quickSaveOneChart(int chartIndex);

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
    void updateInspectorSplitterSizes(bool show);
    void updateExpandActionsVisibility();
    void restoreExpandedSplitterSizes();

    int tabColNumber=0;
    int tabRowNumber=0;
    int m_activeChartIndex = 0;
    int m_expandedIndex = -1;
    QList<int> m_savedColSizes;
    QVector<QList<int>> m_savedRowSizes;
    QString m_quickSaveDir;

    QVector <UWatchChart*> graph;
    std::list<double> XData;
    std::list<double> YData;
    QVector<QPointF> points;

    QWidget *chartsHost = nullptr;
    QSplitter *mainSplitter = nullptr;
    QSplitter *colSplitter;
    QVector <QSplitter*> rowSplitter;
    PlotSettingsSidePanel *settingsPanel = nullptr;
    NMSDK::Plot::PlotDocument m_document;

    Ui::UWatchTab *ui;

    virtual void AUpdateInterface();
    virtual void AClearInterface();
    virtual void ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata);

public slots:
    void createSelectionDialogSlot(int index);
    void layoutOptionTriggered();
    void seriesOptionTriggered();
    void chartsOptionTriggered();
    void openSettingsPanelSlot(int chartIndex, bool seriesPage);
    void onChartActivated(int chartIndex);
    void onExpandToggleRequested(int chartIndex);
    void onSaveChartAsRequested(int chartIndex);
    void onQuickSaveChartRequested(int chartIndex);

};

#endif // UWATCHTAB_H
