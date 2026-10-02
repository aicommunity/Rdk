#ifndef PLOTSETTINGSSIDEPANEL_H
#define PLOTSETTINGSSIDEPANEL_H

#include <QWidget>

class UWatchTab;
class UWatchChart;
class UWatchSerie;
class QComboBox;
class QLineEdit;
class QDoubleSpinBox;
class QCheckBox;
class QListWidget;
class QLabel;
class QPushButton;
class QTabWidget;
class QButtonGroup;

enum class PlotInspectorPage
{
    Chart = 0,
    Series = 1
};

/// Chart/Series inspector docked in UWatchTab splitter (live apply).
class PlotSettingsSidePanel : public QWidget
{
    Q_OBJECT
public:
    explicit PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent = nullptr);

    void setActiveChart(int chartIndex);
    void showPage(PlotInspectorPage page);
    void showChartPage() { showPage(PlotInspectorPage::Chart); }
    void showSeriesPage() { showPage(PlotInspectorPage::Series); }
    void refreshFromTab();

    void showInspector(PlotInspectorPage page, int chartIndex);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void requestHide();
    void requestApply();

private slots:
    void onTabChanged(int index);
    void onSeriesSelectionChanged();
    void applyChartLive();
    void applySeriesLive();
    void applySerieColor(int colorId);

private:
    void buildUi();
    void updateHero();
    void connectLiveApply();
    void updateAxesModeVisibility();

    UWatchTab* m_tab = nullptr;
    int m_chartIndex = 0;
    int m_serieIndex = 0;
    bool m_refreshing = false;

    QLabel* m_heroTitle = nullptr;
    QComboBox* m_panelCombo = nullptr;
    QPushButton* m_expandPanelBtn = nullptr;
    QPushButton* m_hideBtn = nullptr;
    QTabWidget* m_tabs = nullptr;

    QWidget* m_chartPage = nullptr;
    QLineEdit* m_titleEdit = nullptr;
    QLineEdit* m_axisXEdit = nullptr;
    QLineEdit* m_axisYEdit = nullptr;
    QDoubleSpinBox* m_yMin = nullptr;
    QDoubleSpinBox* m_yMax = nullptr;
    QDoubleSpinBox* m_xMin = nullptr;
    QDoubleSpinBox* m_xMax = nullptr;
    QDoubleSpinBox* m_xRange = nullptr;
    QLabel* m_xRangeLabel = nullptr;
    QCheckBox* m_trackLatest = nullptr;
    QCheckBox* m_legendVisible = nullptr;
    QCheckBox* m_titleVisible = nullptr;
    QCheckBox* m_denseGrid = nullptr;
    QCheckBox* m_fixedYRange = nullptr;
    QCheckBox* m_fixedXRange = nullptr;
    QPushButton* m_syncXBtn = nullptr;
    QPushButton* m_resetViewBtn = nullptr;
    QPushButton* m_autoScaleBtn = nullptr;
    QPushButton* m_toggleLegendsBtn = nullptr;
    QPushButton* m_saveTemplateBtn = nullptr;
    QPushButton* m_loadTemplateBtn = nullptr;
    QPushButton* m_panelUpBtn = nullptr;
    QPushButton* m_panelDownBtn = nullptr;
    QComboBox* m_vizKind = nullptr;

    QWidget* m_seriesPage = nullptr;
    QListWidget* m_seriesList = nullptr;
    QLineEdit* m_serieName = nullptr;
    QDoubleSpinBox* m_yShift = nullptr;
    QLabel* m_bindingLabel = nullptr;
    QPushButton* m_addSerieBtn = nullptr;
    QPushButton* m_quickAddBtn = nullptr;
    QPushButton* m_hideSerieBtn = nullptr;
    QPushButton* m_showSerieBtn = nullptr;
    QPushButton* m_dupSerieBtn = nullptr;
    QPushButton* m_delSerieBtn = nullptr;
    QPushButton* m_upSerieBtn = nullptr;
    QPushButton* m_downSerieBtn = nullptr;
    class QSpinBox* m_channelSpin = nullptr;
    class QSpinBox* m_lineWidth = nullptr;
    QComboBox* m_lineStyle = nullptr;
    QPushButton* m_gotoSourceBtn = nullptr;
    QButtonGroup* m_serieColorGroup = nullptr;
    int m_serieColorIndex = -1;

    void syncSerieColorSelection(UWatchChart* chart, UWatchSerie* serie);
};

#endif // PLOTSETTINGSSIDEPANEL_H
