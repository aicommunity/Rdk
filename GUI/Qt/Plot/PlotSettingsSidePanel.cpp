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
#include <QShortcut>
#include <QKeySequence>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QVBoxLayout>

PlotSettingsSidePanel::PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent)
    : QWidget(parent)
    , m_tab(tab)
{
    setObjectName(QStringLiteral("PlotSettingsSidePanel"));
    setMinimumWidth(280);
    setMaximumWidth(380);
    buildUi();
    auto* esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    esc->setContext(Qt::WidgetWithChildrenShortcut);
    connect(esc, &QShortcut::activated, this, &PlotSettingsSidePanel::requestHide);
    hide();
}

void PlotSettingsSidePanel::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(6, 6, 6, 6);

    auto* headerRow = new QHBoxLayout();
    m_headerLabel = new QLabel(this);
    m_headerLabel->setWordWrap(true);
    m_hideBtn = new QPushButton(tr("Hide"), this);
    m_hideBtn->setFixedWidth(56);
    headerRow->addWidget(m_headerLabel, 1);
    headerRow->addWidget(m_hideBtn);
    root->addLayout(headerRow);
    connect(m_hideBtn, &QPushButton::clicked, this, &PlotSettingsSidePanel::requestHide);

    m_activeChartCombo = new QComboBox(this);
    root->addWidget(new QLabel(tr("Active chart"), this));
    root->addWidget(m_activeChartCombo);
    connect(m_activeChartCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlotSettingsSidePanel::onActiveChartComboChanged);

    m_pageCombo = new QComboBox(this);
    m_pageCombo->addItem(tr("Layout"), static_cast<int>(PlotInspectorPage::Layout));
    m_pageCombo->addItem(tr("Chart"), static_cast<int>(PlotInspectorPage::Chart));
    m_pageCombo->addItem(tr("Series"), static_cast<int>(PlotInspectorPage::Series));
    root->addWidget(m_pageCombo);
    connect(m_pageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PlotSettingsSidePanel::onPageComboChanged);

    // --- Layout page (tab scope) ---
    m_layoutPage = new QWidget(this);
    auto* layoutForm = new QFormLayout(m_layoutPage);
    m_gridRows = new QSpinBox(m_layoutPage);
    m_gridCols = new QSpinBox(m_layoutPage);
    m_gridRows->setRange(1, 8);
    m_gridCols->setRange(1, 8);
    m_updateInterval = new QSpinBox(m_layoutPage);
    m_updateInterval->setRange(16, 10000);
    m_updateInterval->setSuffix(QStringLiteral(" ms"));
    layoutForm->addRow(tr("Grid rows"), m_gridRows);
    layoutForm->addRow(tr("Grid cols"), m_gridCols);
    layoutForm->addRow(tr("Update interval"), m_updateInterval);
    auto* applyLayout = new QPushButton(tr("Apply layout"), m_layoutPage);
    layoutForm->addRow(applyLayout);
    connect(applyLayout, &QPushButton::clicked, this, &PlotSettingsSidePanel::onApplyLayout);

    // --- Chart page ---
    m_chartPage = new QWidget(this);
    auto* chartForm = new QFormLayout(m_chartPage);
    m_titleEdit = new QLineEdit(m_chartPage);
    m_axisXEdit = new QLineEdit(m_chartPage);
    m_axisYEdit = new QLineEdit(m_chartPage);
    m_yMin = new QDoubleSpinBox(m_chartPage);
    m_yMax = new QDoubleSpinBox(m_chartPage);
    m_xRange = new QDoubleSpinBox(m_chartPage);
    m_yMin->setRange(-1e9, 1e9);
    m_yMax->setRange(-1e9, 1e9);
    m_xRange->setRange(0.001, 1e9);
    m_xRange->setDecimals(3);
    m_trackLatest = new QCheckBox(tr("Track latest"), m_chartPage);
    m_legendVisible = new QCheckBox(tr("Show legend"), m_chartPage);
    m_titleVisible = new QCheckBox(tr("Show title"), m_chartPage);
    m_vizKind = new QComboBox(m_chartPage);
    m_vizKind->addItem(tr("Time series"), static_cast<int>(NMSDK::Plot::VizKind::TimeSeries));
    m_vizKind->addItem(tr("XY line"), static_cast<int>(NMSDK::Plot::VizKind::XYLine));
    m_vizKind->addItem(tr("XY scatter"), static_cast<int>(NMSDK::Plot::VizKind::XYScatter));
    chartForm->addRow(tr("Title"), m_titleEdit);
    chartForm->addRow(tr("X axis"), m_axisXEdit);
    chartForm->addRow(tr("Y axis"), m_axisYEdit);
    chartForm->addRow(tr("Y min"), m_yMin);
    chartForm->addRow(tr("Y max"), m_yMax);
    chartForm->addRow(tr("X range"), m_xRange);
    chartForm->addRow(tr("Viz kind"), m_vizKind);
    chartForm->addRow(m_trackLatest);
    chartForm->addRow(m_legendVisible);
    chartForm->addRow(m_titleVisible);
    auto* applyChart = new QPushButton(tr("Apply chart"), m_chartPage);
    chartForm->addRow(applyChart);
    connect(applyChart, &QPushButton::clicked, this, &PlotSettingsSidePanel::onApplyChart);

    // --- Series page ---
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

    root->addWidget(m_layoutPage);
    root->addWidget(m_chartPage);
    root->addWidget(m_seriesPage);
    root->addStretch(1);
    updatePageVisibility();
}

void PlotSettingsSidePanel::updatePageVisibility()
{
    const int page = m_pageCombo ? m_pageCombo->currentData().toInt() : 0;
    m_layoutPage->setVisible(page == static_cast<int>(PlotInspectorPage::Layout));
    m_chartPage->setVisible(page == static_cast<int>(PlotInspectorPage::Chart));
    m_seriesPage->setVisible(page == static_cast<int>(PlotInspectorPage::Series));
    // Active chart combo only relevant for Chart/Series
    const bool chartScoped = page != static_cast<int>(PlotInspectorPage::Layout);
    m_activeChartCombo->setEnabled(chartScoped);
}

void PlotSettingsSidePanel::updateHeader()
{
    if (!m_tab)
    {
        m_headerLabel->setText(tr("Inspector"));
        return;
    }
    const int n = m_tab->countGraphs();
    QString chartTitle;
    if (m_chartIndex >= 0 && m_chartIndex < n)
    {
        if (UWatchChart* c = m_tab->getChart(m_chartIndex))
            chartTitle = c->getChartTitle();
    }
    if (chartTitle.isEmpty())
        chartTitle = tr("Chart %1").arg(m_chartIndex + 1);

    const int page = m_pageCombo->currentData().toInt();
    if (page == static_cast<int>(PlotInspectorPage::Layout))
        m_headerLabel->setText(tr("Layout — tab grid & update"));
    else if (page == static_cast<int>(PlotInspectorPage::Series))
        m_headerLabel->setText(tr("Series — Chart %1/%2 — %3")
                                   .arg(m_chartIndex + 1)
                                   .arg(qMax(1, n))
                                   .arg(chartTitle));
    else
        m_headerLabel->setText(tr("Chart %1/%2 — %3")
                                   .arg(m_chartIndex + 1)
                                   .arg(qMax(1, n))
                                   .arg(chartTitle));
}

void PlotSettingsSidePanel::setActiveChart(int chartIndex)
{
    m_chartIndex = chartIndex;
    {
        QSignalBlocker blocker(m_activeChartCombo);
        if (chartIndex >= 0 && chartIndex < m_activeChartCombo->count())
            m_activeChartCombo->setCurrentIndex(chartIndex);
    }
    refreshFromTab();
}

void PlotSettingsSidePanel::showPage(PlotInspectorPage page)
{
    const int idx = m_pageCombo->findData(static_cast<int>(page));
    if (idx >= 0)
        m_pageCombo->setCurrentIndex(idx);
    else
        updatePageVisibility();
    updateHeader();
}

void PlotSettingsSidePanel::showInspector(PlotInspectorPage page, int chartIndex)
{
    if (chartIndex >= 0)
        setActiveChart(chartIndex);
    showPage(page);
    show();
    raise();
}

void PlotSettingsSidePanel::onPageComboChanged(int)
{
    updatePageVisibility();
    refreshFromTab();
    updateHeader();
}

void PlotSettingsSidePanel::onActiveChartComboChanged(int index)
{
    if (index < 0)
        return;
    m_chartIndex = index;
    emit activeChartChanged(index);
    refreshFromTab();
    updateHeader();
}

void PlotSettingsSidePanel::refreshFromTab()
{
    if (!m_tab)
        return;

    m_gridRows->setValue(qMax(1, m_tab->getRowNumber()));
    m_gridCols->setValue(qMax(1, m_tab->getColNumber()));
    m_updateInterval->setValue(m_tab->UpdateIntervalMs);

    {
        QSignalBlocker blocker(m_activeChartCombo);
        m_activeChartCombo->clear();
        for (int i = 0; i < m_tab->countGraphs(); ++i)
        {
            UWatchChart* c = m_tab->getChart(i);
            QString title = c ? c->getChartTitle() : QString();
            if (title.isEmpty())
                title = tr("Chart %1").arg(i + 1);
            m_activeChartCombo->addItem(QStringLiteral("%1 — %2").arg(i + 1).arg(title), i);
        }
        if (m_chartIndex >= 0 && m_chartIndex < m_activeChartCombo->count())
            m_activeChartCombo->setCurrentIndex(m_chartIndex);
        else if (m_activeChartCombo->count() > 0)
        {
            m_chartIndex = 0;
            m_activeChartCombo->setCurrentIndex(0);
        }
    }

    updateHeader();

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

void PlotSettingsSidePanel::onApplyLayout()
{
    if (!m_tab)
        return;
    m_tab->saveUpdateInterval(m_updateInterval->value());
    m_tab->createGridLayout(m_gridRows->value(), m_gridCols->value());
    m_chartIndex = qBound(0, m_chartIndex, m_tab->countGraphs() - 1);
    m_tab->setActiveChart(m_chartIndex);
    m_tab->syncDocumentFromCharts();
    refreshFromTab();
    emit requestApply();
}

void PlotSettingsSidePanel::onApplyChart()
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
    chart->fixInitialAxesState();
    m_tab->syncDocumentFromCharts();
    refreshFromTab();
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
