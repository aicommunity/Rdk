#ifndef PLOT_SETTINGS_SIDE_PANEL_H
#define PLOT_SETTINGS_SIDE_PANEL_H

#include <QWidget>

class UWatchTab;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QListWidget;
class QPushButton;
class QLabel;

enum class PlotInspectorPage
{
    Layout = 0,
    Chart = 1,
    Series = 2
};

/// Collapsible inspector: Layout (tab) | Chart | Series.
class PlotSettingsSidePanel : public QWidget
{
    Q_OBJECT
public:
    explicit PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent = nullptr);

    void setActiveChart(int chartIndex);
    void refreshFromTab();
    void showPage(PlotInspectorPage page);
    void showLayoutPage() { showPage(PlotInspectorPage::Layout); }
    void showChartPage() { showPage(PlotInspectorPage::Chart); }
    void showSeriesPage() { showPage(PlotInspectorPage::Series); }

    /// Open inspector on page for chartIndex (no forced chart 0).
    void showInspector(PlotInspectorPage page, int chartIndex);

signals:
    void requestApply();
    void requestHide();
    void activeChartChanged(int chartIndex);

private slots:
    void onApplyLayout();
    void onApplyChart();
    void onApplySeries();
    void onSeriesSelectionChanged();
    void onActiveChartComboChanged(int index);
    void onPageComboChanged(int index);

private:
    void buildUi();
    void updateHeader();
    void updatePageVisibility();

    UWatchTab* m_tab = nullptr;
    int m_chartIndex = 0;
    int m_serieIndex = 0;

    QLabel* m_headerLabel = nullptr;
    QComboBox* m_activeChartCombo = nullptr;
    QComboBox* m_pageCombo = nullptr;
    QPushButton* m_hideBtn = nullptr;

    QWidget* m_layoutPage = nullptr;
    QWidget* m_chartPage = nullptr;
    QWidget* m_seriesPage = nullptr;

    QSpinBox* m_gridRows = nullptr;
    QSpinBox* m_gridCols = nullptr;
    QSpinBox* m_updateInterval = nullptr;

    QLineEdit* m_titleEdit = nullptr;
    QLineEdit* m_axisXEdit = nullptr;
    QLineEdit* m_axisYEdit = nullptr;
    QDoubleSpinBox* m_yMin = nullptr;
    QDoubleSpinBox* m_yMax = nullptr;
    QDoubleSpinBox* m_xRange = nullptr;
    QCheckBox* m_trackLatest = nullptr;
    QCheckBox* m_legendVisible = nullptr;
    QCheckBox* m_titleVisible = nullptr;
    QComboBox* m_vizKind = nullptr;

    QListWidget* m_seriesList = nullptr;
    QLineEdit* m_serieName = nullptr;
    QSpinBox* m_channelSpin = nullptr;
    QDoubleSpinBox* m_yShift = nullptr;
    QLabel* m_bindingLabel = nullptr;
    QPushButton* m_addSerieBtn = nullptr;
};

#endif // PLOT_SETTINGS_SIDE_PANEL_H
