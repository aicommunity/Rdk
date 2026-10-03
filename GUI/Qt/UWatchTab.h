#ifndef UWATCHTAB_H
#define UWATCHTAB_H

#include <iostream>

#include <QWidget>
#include <QString>
#include <QVector>
#include <QSplitter>
#include <QToolBar>
#include <QAction>

#include "UWatchChart.h"
#include "UVisualControllerWidget.h"
#include "Plot/PlotDocument.h"
#include "Plot/PlotSettingsSidePanel.h"
#include "NmsdkQtCompat.h"

#include <QUndoStack>

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
    /// Charts currently placed in the grid (≤ rows*cols).
    int visibleSlotCount() const { return tabColNumber * tabRowNumber; }
    int activeChartIndex() const { return m_activeChartIndex; }
    void setActiveChart(int index);

    void createSelectionDialog(int chartIndex);
    void openQuickAddDialog(int chartIndex);
    bool restorePanelSnapshot(int insertIndex, const NMSDK::Plot::PlotPanel& panel,
                              bool preservePanelId = false);
    bool movePanelToWatchTab(int chartIndex, int destinationTabIndex);
    void pushSerieEnabledUndo(int chartIndex, int serieIndex, bool enabled);
    void refreshInspectorIfOpen();

    /// Quick Y(t) add without Style page (defaults).
    bool quickAddTimeSeries(int chartIndex,
                            const QString& component,
                            const QString& property,
                            int jx = 0,
                            int jy = 0,
                            int channel = 0);
    /// Copy TimeSeries X window / track from active panel to other TS panels.
    void syncTimeSeriesXRangeFromActive();

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
    PlotInspectorPage currentInspectorPage() const;
    void focusPanelSearch();

    void toggleExpandChart(int chartIndex);
    void collapseExpandedChart();
    bool isChartExpanded() const { return m_expandedIndex >= 0; }
    int expandedChartIndex() const { return m_expandedIndex; }

    void setDenseMode(bool dense);
    bool isDenseMode() const { return m_denseMode; }
    void applyDenseMode(bool dense);
    void syncSharedModeBarFromActive();
    void setAllLegendsVisible(bool visible);

    bool hidePanel(int chartIndex);
    bool showPanel(int chartIndex);
    bool bringPanelIntoGrid(int chartIndex);
    bool duplicatePanel(int chartIndex);
    bool deletePanel(int chartIndex, bool confirm = true);
    bool deletePanelImpl(int chartIndex);
    bool movePanel(int fromIndex, int toIndex);
    bool renamePanel(int chartIndex, const QString& title);
    void pushPanelMoveUndo(int fromIndex, int toIndex);
    void pushSerieDeleteUndo(int chartIndex, int serieIndex);
    void pushSerieDuplicateUndo(int chartIndex, int serieIndex);
    void pushSerieMoveUndo(int chartIndex, int fromIndex, int toIndex);
    QUndoStack* undoStack() { return m_undoStack; }

    /// `<ProjectPath>/SavedWatches/` or empty if no project.
    QString savedWatchesRoot() const;
    /// `<ProjectPath>/WatchTemplates/` or shared Bin/WatchTemplates/.
    QString watchTemplatesDir() const;
    bool saveWatchTemplateAs(const QString& filePath);
    bool loadWatchTemplateFrom(const QString& filePath, bool reassignIds = true);
    void saveWatchTemplateDialog();
    void loadWatchTemplateDialog();
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
    void tearDownSplitterLayout();

    void createSplitterGrid(int rowNumber);
    void ensureSettingsPanel();
    void applySplitterSizes(const NMSDK::Plot::PlotDocument& doc);
    void updateInspectorSplitterSizes(bool show);
    void updateExpandActionsVisibility();
    void restoreExpandedSplitterSizes();
    void ensureSharedModeBar();
    void applyPanelBinding(UWatchChart* chart, const NMSDK::Plot::PlotPanel& panel);

    int tabColNumber=0;
    int tabRowNumber=0;
    int m_activeChartIndex = 0;
    int m_expandedIndex = -1;
    bool m_denseMode = false;
    QList<int> m_savedColSizes;
    QVector<QList<int>> m_savedRowSizes;
    QString m_quickSaveDir;

    QVector <UWatchChart*> graph;
    std::list<double> XData;
    std::list<double> YData;
    QVector<QPointF> points;

    QWidget *chartsHost = nullptr;
    QWidget *chartsColumn = nullptr;
    QToolBar *m_sharedModeBar = nullptr;
    QAction *m_sharedPan = nullptr;
    QAction *m_sharedBoxZoom = nullptr;
    QAction *m_sharedTrack = nullptr;
    QAction *m_sharedReset = nullptr;
    QAction *m_sharedExpand = nullptr;
    QSplitter *mainSplitter = nullptr;
    QSplitter *colSplitter;
    QVector <QSplitter*> rowSplitter;
    PlotSettingsSidePanel *settingsPanel = nullptr;
    NMSDK::Plot::PlotDocument m_document;
    QUndoStack* m_undoStack = nullptr;

    Ui::UWatchTab *ui;

    virtual void AUpdateInterface();
    virtual void AClearInterface();
    virtual void ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata);

signals:
    void inspectorStateChanged();

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
    void onDuplicatePanelRequested(int chartIndex);
    void onHidePanelRequested(int chartIndex);
    void onDeletePanelRequested(int chartIndex);

};

#endif // UWATCHTAB_H
