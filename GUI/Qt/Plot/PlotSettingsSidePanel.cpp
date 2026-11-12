#include "PlotSettingsSidePanel.h"

#include "../UWatchTab.h"
#include "../UWatchChart.h"
#include "../UWatchSerie.h"
#include "../UStyleManager.h"
#include "PlotDocument.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

void styleForm(QFormLayout* form)
{
    form->setContentsMargins(8, 8, 8, 8);
    form->setSpacing(6);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::WrapLongRows);
}

} // namespace

PlotSettingsSidePanel::PlotSettingsSidePanel(UWatchTab* tab, QWidget* parent)
    : QWidget(parent)
    , m_tab(tab)
{
    setObjectName(QStringLiteral("PlotSettingsSidePanel"));
    setMinimumWidth(320);
    setMaximumWidth(420);
    buildUi();
    connectLiveApply();
    auto* esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    esc->setContext(Qt::WidgetWithChildrenShortcut);
    connect(esc, &QShortcut::activated, this, &PlotSettingsSidePanel::requestHide);
    hide();
}

void PlotSettingsSidePanel::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    // --- Hero identity (read-only; active chart is chosen by click in the grid) ---
    auto* heroRow = new QHBoxLayout();
    m_heroTitle = new QLabel(this);
    QFont heroFont = m_heroTitle->font();
    heroFont.setPointSize(heroFont.pointSize() + 2);
    heroFont.setBold(true);
    m_heroTitle->setFont(heroFont);
    m_heroTitle->setWordWrap(true);
    heroRow->addWidget(m_heroTitle, 1);
    m_hideBtn = new QPushButton(tr("Hide"), this);
    m_hideBtn->setFixedWidth(56);
    heroRow->addWidget(m_hideBtn, 0, Qt::AlignTop);
    root->addLayout(heroRow);
    connect(m_hideBtn, &QPushButton::clicked, this, &PlotSettingsSidePanel::requestHide);

    m_tabs = new QTabWidget(this);
    m_tabs->setDocumentMode(true);
    m_tabs->setElideMode(Qt::ElideNone);

    // --- Chart page ---
    m_chartPage = new QWidget(m_tabs);
    auto* chartLayout = new QVBoxLayout(m_chartPage);
    chartLayout->setContentsMargins(4, 8, 4, 4);
    chartLayout->setSpacing(8);

    auto* identityBox = new QGroupBox(tr("Identity"), m_chartPage);
    auto* identityForm = new QFormLayout(identityBox);
    styleForm(identityForm);
    m_titleEdit = new QLineEdit(identityBox);
    m_vizKind = new QComboBox(identityBox);
    m_vizKind->addItem(tr("Time series"), static_cast<int>(NMSDK::Plot::VizKind::TimeSeries));
    m_vizKind->addItem(tr("XY line"), static_cast<int>(NMSDK::Plot::VizKind::XYLine));
    m_vizKind->addItem(tr("XY scatter"), static_cast<int>(NMSDK::Plot::VizKind::XYScatter));
    identityForm->addRow(tr("Title"), m_titleEdit);
    identityForm->addRow(tr("Viz kind"), m_vizKind);
    chartLayout->addWidget(identityBox);

    auto* axesBox = new QGroupBox(tr("Axes"), m_chartPage);
    auto* axesForm = new QFormLayout(axesBox);
    styleForm(axesForm);
    m_axisXEdit = new QLineEdit(axesBox);
    m_axisYEdit = new QLineEdit(axesBox);
    m_yMin = new QDoubleSpinBox(axesBox);
    m_yMax = new QDoubleSpinBox(axesBox);
    m_xMin = new QDoubleSpinBox(axesBox);
    m_xMax = new QDoubleSpinBox(axesBox);
    m_xRange = new QDoubleSpinBox(axesBox);
    const QLocale cLocale = QLocale::c();
    m_yMin->setLocale(cLocale);
    m_yMax->setLocale(cLocale);
    m_xMin->setLocale(cLocale);
    m_xMax->setLocale(cLocale);
    m_xRange->setLocale(cLocale);
    m_yMin->setRange(-1e9, 1e9);
    m_yMax->setRange(-1e9, 1e9);
    m_xMin->setRange(-1e9, 1e9);
    m_xMax->setRange(-1e9, 1e9);
    m_xMin->setDecimals(6);
    m_xMax->setDecimals(6);
    m_yMin->setDecimals(6);
    m_yMax->setDecimals(6);
    m_xMin->setMinimumWidth(120);
    m_xMax->setMinimumWidth(120);
    m_yMin->setMinimumWidth(120);
    m_yMax->setMinimumWidth(120);
    m_xRange->setRange(0.001, 1e9);
    m_xRange->setDecimals(3);
    axesForm->addRow(tr("X axis"), m_axisXEdit);
    axesForm->addRow(tr("Y axis"), m_axisYEdit);
    axesForm->addRow(tr("X min"), m_xMin);
    axesForm->addRow(tr("X max"), m_xMax);
    axesForm->addRow(tr("Y min"), m_yMin);
    axesForm->addRow(tr("Y max"), m_yMax);
    axesForm->addRow(tr("X range"), m_xRange);
    m_xRangeLabel = qobject_cast<QLabel*>(axesForm->labelForField(m_xRange));
    chartLayout->addWidget(axesBox);

    auto* displayBox = new QGroupBox(tr("Display"), m_chartPage);
    auto* displayLayout = new QVBoxLayout(displayBox);
    displayLayout->setContentsMargins(8, 8, 8, 8);
    m_trackLatest = new QCheckBox(tr("Track latest"), displayBox);
    m_legendVisible = new QCheckBox(tr("Show legend"), displayBox);
    m_titleVisible = new QCheckBox(tr("Show title"), displayBox);
    displayLayout->addWidget(m_trackLatest);
    displayLayout->addWidget(m_legendVisible);
    displayLayout->addWidget(m_titleVisible);
    chartLayout->addWidget(displayBox);
    chartLayout->addStretch(1);

    // --- Series page ---
    m_seriesPage = new QWidget(m_tabs);
    auto* seriesLayout = new QVBoxLayout(m_seriesPage);
    seriesLayout->setContentsMargins(4, 8, 4, 4);
    seriesLayout->setSpacing(8);

    auto* listBox = new QGroupBox(tr("Series"), m_seriesPage);
    auto* listLayout = new QVBoxLayout(listBox);
    listLayout->setContentsMargins(8, 8, 8, 8);
    m_seriesList = new QListWidget(listBox);
    m_seriesList->setMinimumHeight(120);
    listLayout->addWidget(m_seriesList);
    seriesLayout->addWidget(listBox, 1);

    auto* selectedBox = new QGroupBox(tr("Selected series"), m_seriesPage);
    auto* selectedForm = new QFormLayout(selectedBox);
    styleForm(selectedForm);
    m_serieName = new QLineEdit(selectedBox);
    m_channelSpin = new QSpinBox(selectedBox);
    m_channelSpin->setRange(0, 255);
    m_yShift = new QDoubleSpinBox(selectedBox);
    m_yShift->setLocale(QLocale::c());
    m_yShift->setRange(-1e9, 1e9);
    m_bindingLabel = new QLabel(selectedBox);
    m_bindingLabel->setWordWrap(true);
    m_bindingLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    selectedForm->addRow(tr("Name"), m_serieName);
    selectedForm->addRow(tr("Channel"), m_channelSpin);
    selectedForm->addRow(tr("Y shift"), m_yShift);
    selectedForm->addRow(tr("Binding"), m_bindingLabel);

    auto* colorRow = new QHBoxLayout();
    m_serieColorGroup = new QButtonGroup(selectedBox);
    m_serieColorGroup->setExclusive(true);
    auto* autoBtn = new QToolButton(selectedBox);
    autoBtn->setText(tr("Auto"));
    autoBtn->setCheckable(true);
    autoBtn->setChecked(true);
    autoBtn->setToolTip(tr("Next unused palette color"));
    m_serieColorGroup->addButton(autoBtn, -1);
    colorRow->addWidget(autoBtn);
    const int paletteCount = UStyleManager::instance()->getChartSeriesColorCount();
    const int n = qMin(12, qMax(1, paletteCount));
    for (int i = 0; i < n; ++i)
    {
        const QColor c = UStyleManager::instance()->getChartSeriesColor(i);
        QPixmap px(18, 18);
        px.fill(c);
        auto* btn = new QToolButton(selectedBox);
        btn->setIcon(QIcon(px));
        btn->setIconSize(QSize(18, 18));
        btn->setCheckable(true);
        btn->setToolTip(tr("Palette %1").arg(i + 1));
        m_serieColorGroup->addButton(btn, i);
        colorRow->addWidget(btn);
    }
    colorRow->addStretch(1);
    selectedForm->addRow(tr("Color"), colorRow);

    seriesLayout->addWidget(selectedBox);

    m_addSerieBtn = new QPushButton(tr("Add series…"), m_seriesPage);
    seriesLayout->addWidget(m_addSerieBtn);
    connect(m_addSerieBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->createSelectionDialog(m_chartIndex);
        refreshFromTab();
        if (m_tab)
            m_tab->syncDocumentFromCharts();
    });
    connect(m_seriesList, &QListWidget::currentRowChanged,
            this, &PlotSettingsSidePanel::onSeriesSelectionChanged);

    m_tabs->addTab(m_chartPage, tr("Chart"));
    m_tabs->addTab(m_seriesPage, tr("Series"));
    connect(m_tabs, &QTabWidget::currentChanged, this, &PlotSettingsSidePanel::onTabChanged);
    root->addWidget(m_tabs, 1);
}

void PlotSettingsSidePanel::connectLiveApply()
{
    auto chartLive = [this]() { applyChartLive(); };
    auto seriesLive = [this]() { applySeriesLive(); };

    connect(m_titleEdit, &QLineEdit::editingFinished, this, chartLive);
    connect(m_axisXEdit, &QLineEdit::editingFinished, this, chartLive);
    connect(m_axisYEdit, &QLineEdit::editingFinished, this, chartLive);
    connect(m_yMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, chartLive);
    connect(m_yMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, chartLive);
    connect(m_xMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, chartLive);
    connect(m_xMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, chartLive);
    connect(m_xRange, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, chartLive);
    connect(m_vizKind, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, chartLive]() {
        updateAxesModeVisibility();
        chartLive();
    });
    connect(m_trackLatest, &QCheckBox::toggled, this, chartLive);
    connect(m_legendVisible, &QCheckBox::toggled, this, chartLive);
    connect(m_titleVisible, &QCheckBox::toggled, this, chartLive);

    connect(m_serieName, &QLineEdit::editingFinished, this, seriesLive);
    connect(m_channelSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, seriesLive);
    connect(m_yShift, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, seriesLive);
    if (m_serieColorGroup)
    {
        connect(m_serieColorGroup, QOverload<int>::of(&QButtonGroup::idClicked),
                this, &PlotSettingsSidePanel::applySerieColor);
    }
}

void PlotSettingsSidePanel::updateHero()
{
    if (!m_tab || m_tab->countGraphs() <= 0)
    {
        m_heroTitle->setText(tr("No chart"));
        return;
    }
    m_heroTitle->setText(tr("Chart %1").arg(m_chartIndex + 1));
}

void PlotSettingsSidePanel::setActiveChart(int chartIndex)
{
    m_chartIndex = chartIndex;
    refreshFromTab();
}

void PlotSettingsSidePanel::showPage(PlotInspectorPage page)
{
    if (!m_tabs)
        return;
    const int idx = static_cast<int>(page);
    if (idx >= 0 && idx < m_tabs->count())
        m_tabs->setCurrentIndex(idx);
    updateHero();
}

void PlotSettingsSidePanel::showInspector(PlotInspectorPage page, int chartIndex)
{
    if (chartIndex >= 0)
        setActiveChart(chartIndex);
    showPage(page);
    show();
    raise();
}

void PlotSettingsSidePanel::onTabChanged(int)
{
    refreshFromTab();
    updateHero();
}

void PlotSettingsSidePanel::updateAxesModeVisibility()
{
    const auto viz = static_cast<NMSDK::Plot::VizKind>(
        m_vizKind ? m_vizKind->currentData().toInt() : 0);
    const bool xy = NMSDK::Plot::isXYFamily(viz);
    if (m_xMin)
        m_xMin->setVisible(xy);
    if (m_xMax)
        m_xMax->setVisible(xy);
    if (m_xRange)
        m_xRange->setVisible(!xy);
    if (m_xRangeLabel)
        m_xRangeLabel->setVisible(!xy);
    if (m_trackLatest)
        m_trackLatest->setVisible(!xy);
    // Show labels for X min/max when XY
    if (m_xMin && m_xMin->parentWidget())
    {
        if (auto* form = qobject_cast<QFormLayout*>(m_xMin->parentWidget()->layout()))
        {
            if (QWidget* lab = form->labelForField(m_xMin))
                lab->setVisible(xy);
            if (QWidget* lab = form->labelForField(m_xMax))
                lab->setVisible(xy);
        }
    }
}

void PlotSettingsSidePanel::refreshFromTab()
{
    if (!m_tab)
        return;

    m_refreshing = true;

    if (m_tab->countGraphs() <= 0)
    {
        m_chartIndex = 0;
        updateHero();
        m_refreshing = false;
        return;
    }
    if (m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        m_chartIndex = 0;

    updateHero();

    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
    {
        m_refreshing = false;
        return;
    }

    {
        QSignalBlocker b1(m_titleEdit);
        QSignalBlocker b2(m_axisXEdit);
        QSignalBlocker b3(m_axisYEdit);
        QSignalBlocker b4(m_yMin);
        QSignalBlocker b5(m_yMax);
        QSignalBlocker bXmin(m_xMin);
        QSignalBlocker bXmax(m_xMax);
        QSignalBlocker b6(m_xRange);
        QSignalBlocker b7(m_trackLatest);
        QSignalBlocker b8(m_legendVisible);
        QSignalBlocker b9(m_titleVisible);
        QSignalBlocker b10(m_vizKind);

        m_titleEdit->setText(chart->getChartTitle());
        m_axisXEdit->setText(chart->getAxisXName());
        m_axisYEdit->setText(chart->getAxisYName());
        m_yMin->setValue(chart->getAxisYmin());
        m_yMax->setValue(chart->getAxisYmax());
        m_xMin->setValue(chart->getAxisXmin());
        m_xMax->setValue(chart->getAxisXmax());
        m_xRange->setValue(chart->getAxisXrange());
        m_trackLatest->setChecked(chart->getIsAxisXtrackable());
        m_legendVisible->setChecked(chart->isLegendVisible());
        m_titleVisible->setChecked(chart->isTitleVisible());

        const int viz = static_cast<int>(chart->getVizKind());
        const int vizIdx = m_vizKind->findData(viz);
        if (vizIdx >= 0)
            m_vizKind->setCurrentIndex(vizIdx);
    }
    updateAxesModeVisibility();

    {
        QSignalBlocker blocker(m_seriesList);
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
        }
    }
    onSeriesSelectionChanged();

    m_refreshing = false;
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

    QSignalBlocker b1(m_serieName);
    QSignalBlocker b2(m_channelSpin);
    QSignalBlocker b3(m_yShift);
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

    syncSerieColorSelection(chart, s);
}

void PlotSettingsSidePanel::syncSerieColorSelection(UWatchChart* chart, UWatchSerie* serie)
{
    if (!m_serieColorGroup || !chart || !serie)
        return;
    const QRgb rgb = serie->color().rgb();
    int matched = -1;
    const int paletteCount = UStyleManager::instance()->getChartSeriesColorCount();
    for (int i = 0; i < paletteCount; ++i)
    {
        if (chart->getDefaultColor(i).rgb() == rgb)
        {
            matched = i;
            break;
        }
    }
    m_serieColorIndex = matched;
    QSignalBlocker blocker(m_serieColorGroup);
    if (QAbstractButton* btn = m_serieColorGroup->button(matched))
        btn->setChecked(true);
    else if (QAbstractButton* autoBtn = m_serieColorGroup->button(-1))
        autoBtn->setChecked(true);
}

void PlotSettingsSidePanel::applySerieColor(int colorId)
{
    if (m_refreshing || !m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;
    if (m_serieIndex < 0 || m_serieIndex >= chart->countSeries())
        return;

    m_serieColorIndex = colorId;
    if (colorId < 0)
        chart->setSerieColor(m_serieIndex, chart->suggestAutoColorIndex(m_serieIndex));
    else
        chart->setSerieColor(m_serieIndex, colorId);
    m_tab->syncDocumentFromCharts();
    emit requestApply();
}

void PlotSettingsSidePanel::applyChartLive()
{
    if (m_refreshing || !m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
        return;
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    if (!chart)
        return;

    chart->setChartTitle(m_titleEdit->text());
    chart->setAxisXname(m_axisXEdit->text());
    chart->setAxisYname(m_axisYEdit->text());
    chart->setAxisYmin(m_yMin->value());
    chart->setAxisYmax(m_yMax->value());
    const auto viz = static_cast<NMSDK::Plot::VizKind>(m_vizKind->currentData().toInt());
    if (chart->countSeries() > 0 && !NMSDK::Plot::sameVizFamily(chart->getVizKind(), viz))
    {
        QSignalBlocker b(m_vizKind);
        const int idx = m_vizKind->findData(static_cast<int>(chart->getVizKind()));
        if (idx >= 0)
            m_vizKind->setCurrentIndex(idx);
        updateAxesModeVisibility();
        QMessageBox::warning(this, tr("Watch"),
                             tr("Cannot switch between Time series and Y(x) while the chart has series. "
                                "Clear series or use another chart."));
        return;
    }
    const bool xy = NMSDK::Plot::isXYFamily(viz);
    if (xy)
    {
        chart->setAxisXmin(m_xMin->value());
        chart->setAxisXmax(m_xMax->value());
        chart->isAxisXtrackable = false;
    }
    else
    {
        chart->updateTimeIntervals(m_xRange->value());
        chart->isAxisXtrackable = m_trackLatest->isChecked();
    }
    chart->setLegendVisible(m_legendVisible->isChecked());
    chart->setTitleVisible(m_titleVisible->isChecked());
    chart->setVizKind(viz);
    chart->fixInitialAxesState();
    m_tab->syncDocumentFromCharts();
    updateHero();
    emit requestApply();
}

void PlotSettingsSidePanel::applySeriesLive()
{
    if (m_refreshing || !m_tab || m_chartIndex < 0 || m_chartIndex >= m_tab->countGraphs())
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
    if (m_serieIndex >= 0 && m_serieIndex < m_seriesList->count())
    {
        QSignalBlocker blocker(m_seriesList);
        m_seriesList->item(m_serieIndex)->setText(m_serieName->text());
    }
    emit requestApply();
}
