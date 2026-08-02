#include "PlotSettingsSidePanel.h"

#include "../UWatchTab.h"
#include "../UWatchChart.h"
#include "../UWatchSerie.h"
#include "PlotDocument.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PlotSettingsSidePanel::PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent)
    : QWidget(parent)
    , m_tab(tab)
{
    setObjectName(QStringLiteral("PlotSettingsSidePanel"));
    setMinimumWidth(260);
    setMaximumWidth(360);
    buildUi();
}

void PlotSettingsSidePanel::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(6, 6, 6, 6);

    m_pageCombo = new QComboBox(this);
    m_pageCombo->addItem(tr("Panel"));
    m_pageCombo->addItem(tr("Series"));
    root->addWidget(m_pageCombo);

    m_panelPage = new QWidget(this);
    auto* panelForm = new QFormLayout(m_panelPage);
    m_gridRows = new QSpinBox(m_panelPage);
    m_gridCols = new QSpinBox(m_panelPage);
    m_gridRows->setRange(1, 8);
    m_gridCols->setRange(1, 8);
    m_titleEdit = new QLineEdit(m_panelPage);
    m_axisXEdit = new QLineEdit(m_panelPage);
    m_axisYEdit = new QLineEdit(m_panelPage);
    m_yMin = new QDoubleSpinBox(m_panelPage);
    m_yMax = new QDoubleSpinBox(m_panelPage);
    m_xRange = new QDoubleSpinBox(m_panelPage);
    m_yMin->setRange(-1e9, 1e9);
    m_yMax->setRange(-1e9, 1e9);
    m_xRange->setRange(0.001, 1e9);
    m_xRange->setDecimals(3);
    m_trackLatest = new QCheckBox(tr("Track latest"), m_panelPage);
    m_legendVisible = new QCheckBox(tr("Show legend"), m_panelPage);
    m_titleVisible = new QCheckBox(tr("Show title"), m_panelPage);
    m_vizKind = new QComboBox(m_panelPage);
    m_vizKind->addItem(tr("Time series"), static_cast<int>(NMSDK::Plot::VizKind::TimeSeries));
    m_vizKind->addItem(tr("XY line"), static_cast<int>(NMSDK::Plot::VizKind::XYLine));
    m_vizKind->addItem(tr("XY scatter"), static_cast<int>(NMSDK::Plot::VizKind::XYScatter));
    m_updateInterval = new QSpinBox(m_panelPage);
    m_updateInterval->setRange(16, 10000);
    m_updateInterval->setSuffix(QStringLiteral(" ms"));

    panelForm->addRow(tr("Grid rows"), m_gridRows);
    panelForm->addRow(tr("Grid cols"), m_gridCols);
    auto* applyGrid = new QPushButton(tr("Apply grid"), m_panelPage);
    panelForm->addRow(applyGrid);
    connect(applyGrid, &QPushButton::clicked, this, &PlotSettingsSidePanel::onApplyGrid);

    panelForm->addRow(tr("Title"), m_titleEdit);
    panelForm->addRow(tr("X axis"), m_axisXEdit);
    panelForm->addRow(tr("Y axis"), m_axisYEdit);
    panelForm->addRow(tr("Y min"), m_yMin);
    panelForm->addRow(tr("Y max"), m_yMax);
    panelForm->addRow(tr("X range"), m_xRange);
    panelForm->addRow(tr("Viz kind"), m_vizKind);
    panelForm->addRow(tr("Update"), m_updateInterval);
    panelForm->addRow(m_trackLatest);
    panelForm->addRow(m_legendVisible);
    panelForm->addRow(m_titleVisible);

    auto* applyPanel = new QPushButton(tr("Apply panel"), m_panelPage);
    panelForm->addRow(applyPanel);
    connect(applyPanel, &QPushButton::clicked, this, &PlotSettingsSidePanel::onApplyPanel);
    connect(m_vizKind, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlotSettingsSidePanel::onVizKindChanged);

    m_seriesPage = new QWidget(this);
    auto* seriesLayout = new QVBoxLayout(m_seriesPage);
    m_seriesList = new QListWidget(m_seriesPage);
    seriesLayout->addWidget(m_seriesList);
    auto* seriesForm = new QFormLayout();
    m_serieName = new QLineEdit(m_seriesPage);
    m_channelSpin = new QSpinBox(m_seriesPage);
    m_channelSpin->setRange(0, 255);
    m_yShift = new QDoubleSpinBox(m_seriesPage);
    m_yShift->setRange(-1e9, 1e9);
    m_bindingLabel = new QLabel(m_seriesPage);
    m_bindingLabel->setWordWrap(true);
    seriesForm->addRow(tr("Name"), m_serieName);
    seriesForm->addRow(tr("Channel"), m_channelSpin);
    seriesForm->addRow(tr("Y shift"), m_yShift);
    seriesForm->addRow(tr("Binding"), m_bindingLabel);
    seriesLayout->addLayout(seriesForm);
    m_addSerieBtn = new QPushButton(tr("Add series…"), m_seriesPage);
    auto* applySeries = new QPushButton(tr("Apply series"), m_seriesPage);
    seriesLayout->addWidget(m_addSerieBtn);
    seriesLayout->addWidget(applySeries);
    connect(applySeries, &QPushButton::clicked, this, &PlotSettingsSidePanel::onApplySeries);
    connect(m_addSerieBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->createSelectionDialog(m_chartIndex);
        refreshFromTab();
        if (m_tab)
            m_tab->syncDocumentFromCharts();
    });
    connect(m_seriesList, &QListWidget::currentRowChanged,
            this, &PlotSettingsSidePanel::onSeriesSelectionChanged);

    root->addWidget(m_panelPage);
    root->addWidget(m_seriesPage);
    m_seriesPage->hide();

    connect(m_pageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int i) {
        m_panelPage->setVisible(i == 0);
        m_seriesPage->setVisible(i == 1);
        if (i == 1)
            refreshFromTab();
    });
}

void PlotSettingsSidePanel::setActiveChart(int chartIndex)
{
    m_chartIndex = chartIndex;
    refreshFromTab();
}

void PlotSettingsSidePanel::showPanelPage()
{
    m_pageCombo->setCurrentIndex(0);
}

void PlotSettingsSidePanel::showSeriesPage()
{
    m_pageCombo->setCurrentIndex(1);
}

void PlotSettingsSidePanel::refreshFromTab()
{
    if (!m_tab)
        return;
    m_gridRows->setValue(qMax(1, m_tab->getRowNumber()));
    m_gridCols->setValue(qMax(1, m_tab->getColNumber()));
    if (m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;

    m_titleEdit->setText(chart->getChartTitle());
    m_axisXEdit->setText(chart->getAxisXName());
    m_axisYEdit->setText(chart->getAxisYName());
    m_yMin->setValue(chart->getAxisYmin());
    m_yMax->setValue(chart->getAxisYmax());
    m_xRange->setValue(chart->getAxisXrange());
    m_trackLatest->setChecked(chart->getIsAxisXtrackable());
    m_legendVisible->setChecked(chart->isLegendVisible());
    m_titleVisible->setChecked(chart->isTitleVisible());
    m_updateInterval->setValue(m_tab->UpdateIntervalMs);

    const int viz = static_cast<int>(chart->getVizKind());
    const int vizIdx = m_vizKind->findData(viz);
    if (vizIdx >= 0)
        m_vizKind->setCurrentIndex(vizIdx);

    m_seriesList->clear();
    for (int i = 0; i < chart->countSeries(); ++i)
    {
        UWatchSerie* s = chart->getSerie(i);
        m_seriesList->addItem(s ? s->name() : QStringLiteral("serie_%1").arg(i));
    }
    if (chart->countSeries() > 0)
    {
        if (m_serieIndex < 0 || m_serieIndex >= chart->countSeries())
            m_serieIndex = 0;
        m_seriesList->setCurrentRow(m_serieIndex);
        onSeriesSelectionChanged();
    }
}

void PlotSettingsSidePanel::onSeriesSelectionChanged()
{
    if (!m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;
    m_serieIndex = m_seriesList->currentRow();
    if (m_serieIndex < 0 || m_serieIndex >= chart->countSeries())
        return;
    UWatchSerie* s = chart->getSerie(m_serieIndex);
    if (!s)
        return;
    m_serieName->setText(s->name());
    m_channelSpin->setValue(s->indexChannel);
    m_yShift->setValue(s->YShift);
    QString binding = QStringLiteral("Y: %1.%2[%3,%4]")
                          .arg(s->nameComponent, s->nameProperty)
                          .arg(s->Jx)
                          .arg(s->Jy);
    if (!s->xNameComponent.isEmpty())
    {
        binding += QStringLiteral("\nX: %1.%2[%3,%4]")
                       .arg(s->xNameComponent, s->xNameProperty)
                       .arg(s->xJx)
                       .arg(s->xJy);
    }
    else
    {
        binding += QStringLiteral("\nX: time");
    }
    m_bindingLabel->setText(binding);
}

void PlotSettingsSidePanel::onApplyGrid()
{
    if (!m_tab)
        return;
    m_tab->createGridLayout(m_gridRows->value(), m_gridCols->value());
    m_chartIndex = 0;
    m_tab->syncDocumentFromCharts();
    refreshFromTab();
    emit requestApply();
}

void PlotSettingsSidePanel::onApplyPanel()
{
    if (!m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;

    chart->setChartTitle(m_titleEdit->text());
    chart->setAxisXname(m_axisXEdit->text());
    chart->setAxisYname(m_axisYEdit->text());
    chart->setAxisYmin(m_yMin->value());
    chart->setAxisYmax(m_yMax->value());
    chart->updateTimeIntervals(m_xRange->value());
    chart->isAxisXtrackable = m_trackLatest->isChecked();
    chart->setLegendVisible(m_legendVisible->isChecked());
    chart->setTitleVisible(m_titleVisible->isChecked());
    chart->setVizKind(static_cast<NMSDK::Plot::VizKind>(m_vizKind->currentData().toInt()));
    m_tab->saveUpdateInterval(m_updateInterval->value());
    chart->fixInitialAxesState();
    m_tab->syncDocumentFromCharts();
    emit requestApply();
}

void PlotSettingsSidePanel::onApplySeries()
{
    if (!m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;
    if (m_serieIndex < 0 || m_serieIndex >= chart->countSeries())
        return;
    UWatchSerie* s = chart->getSerie(m_serieIndex);
    if (!s)
        return;
    chart->setSerieName(m_serieIndex, m_serieName->text());
    s->indexChannel = m_channelSpin->value();
    chart->setSerieYshift(m_serieIndex, static_cast<int>(m_yShift->value()));
    m_tab->syncDocumentFromCharts();
    refreshFromTab();
    emit requestApply();
}

void PlotSettingsSidePanel::onVizKindChanged(int)
{
}
