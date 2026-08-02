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

/// Side inspector: Panel settings + Series settings (replaces overlapping dialogs).
class PlotSettingsSidePanel : public QWidget
{
    Q_OBJECT
public:
    explicit PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent = nullptr);

    void setActiveChart(int chartIndex);
    void refreshFromTab();
    void showPanelPage();
    void showSeriesPage();

signals:
    void requestApply();

private slots:
    void onApplyPanel();
    void onApplySeries();
    void onApplyGrid();
    void onSeriesSelectionChanged();
    void onVizKindChanged(int index);

private:
    void buildUi();
    UWatchTab* m_tab = nullptr;
    int m_chartIndex = 0;
    int m_serieIndex = 0;

    QComboBox* m_pageCombo = nullptr;
    QWidget* m_panelPage = nullptr;
    QWidget* m_seriesPage = nullptr;

    QSpinBox* m_gridRows = nullptr;
    QSpinBox* m_gridCols = nullptr;
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
    QSpinBox* m_updateInterval = nullptr;

    QListWidget* m_seriesList = nullptr;
    QLineEdit* m_serieName = nullptr;
    QSpinBox* m_channelSpin = nullptr;
    QDoubleSpinBox* m_yShift = nullptr;
    QLabel* m_bindingLabel = nullptr;
    QPushButton* m_addSerieBtn = nullptr;
};

#endif // PLOT_SETTINGS_SIDE_PANEL_H
