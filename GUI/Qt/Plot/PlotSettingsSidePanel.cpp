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
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>

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

    // --- Hero: path + panel selector ---
    auto* heroCol = new QVBoxLayout();
    m_heroTitle = new QLabel(this);
    QFont heroFont = m_heroTitle->font();
    heroFont.setPointSize(heroFont.pointSize() + 1);
    heroFont.setBold(true);
    m_heroTitle->setFont(heroFont);
    m_heroTitle->setWordWrap(true);
    heroCol->addWidget(m_heroTitle);
    m_panelCombo = new QComboBox(this);
    m_panelCombo->setToolTip(tr("Select panel without clicking the grid"));
    heroCol->addWidget(m_panelCombo);
    auto* heroRow = new QHBoxLayout();
    heroRow->addLayout(heroCol, 1);
    m_expandPanelBtn = new QPushButton(tr("Expand"), this);
    m_expandPanelBtn->setToolTip(tr("Expand active panel (double-click in list)"));
    m_hideBtn = new QPushButton(tr("Hide"), this);
    m_hideBtn->setFixedWidth(56);
    heroRow->addWidget(m_expandPanelBtn, 0, Qt::AlignTop);
    heroRow->addWidget(m_hideBtn, 0, Qt::AlignTop);
    connect(m_expandPanelBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->toggleExpandChart(m_chartIndex);
    });
    root->addLayout(heroRow);
    connect(m_hideBtn, &QPushButton::clicked, this, &PlotSettingsSidePanel::requestHide);
    connect(m_panelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (m_refreshing || !m_tab || idx < 0)
            return;
        const int chartIdx = m_panelCombo->itemData(idx).toInt();
        m_tab->setActiveChart(chartIdx);
        setActiveChart(chartIdx);
    });
    m_panelCombo->installEventFilter(this);

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
    auto* panelOrderRow = new QHBoxLayout();
    m_panelUpBtn = new QPushButton(tr("Panel ↑"), identityBox);
    m_panelDownBtn = new QPushButton(tr("Panel ↓"), identityBox);
    m_panelUpBtn->setToolTip(tr("Move panel earlier in the document order"));
    m_panelDownBtn->setToolTip(tr("Move panel later in the document order"));
    panelOrderRow->addWidget(m_panelUpBtn);
    panelOrderRow->addWidget(m_panelDownBtn);
    identityForm->addRow(tr("Order"), panelOrderRow);
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
    m_fixedYRange = new QCheckBox(tr("Fixed Y range"), axesBox);
    m_fixedXRange = new QCheckBox(tr("Fixed X range (XY)"), axesBox);
    axesForm->addRow(QString(), m_fixedYRange);
    axesForm->addRow(QString(), m_fixedXRange);
    axesForm->addRow(tr("X min"), m_xMin);
    axesForm->addRow(tr("X max"), m_xMax);
    axesForm->addRow(tr("Y min"), m_yMin);
    axesForm->addRow(tr("Y max"), m_yMax);
    axesForm->addRow(tr("X range / window, s"), m_xRange);
    m_xRangeLabel = qobject_cast<QLabel*>(axesForm->labelForField(m_xRange));
    chartLayout->addWidget(axesBox);

    auto* displayBox = new QGroupBox(tr("Display"), m_chartPage);
    auto* displayLayout = new QVBoxLayout(displayBox);
    displayLayout->setContentsMargins(8, 8, 8, 8);
    m_trackLatest = new QCheckBox(tr("Track latest / follow last data"), displayBox);
    m_legendVisible = new QCheckBox(tr("Show legend"), displayBox);
    m_titleVisible = new QCheckBox(tr("Show title"), displayBox);
    m_denseGrid = new QCheckBox(tr("Dense grid chrome"), displayBox);
    m_syncXBtn = new QPushButton(tr("Sync X from active"), displayBox);
    m_resetViewBtn = new QPushButton(tr("Reset view"), displayBox);
    m_autoScaleBtn = new QPushButton(tr("Auto-scale Y"), displayBox);
    m_toggleLegendsBtn = new QPushButton(tr("Toggle all legends"), displayBox);
    displayLayout->addWidget(m_trackLatest);
    displayLayout->addWidget(m_legendVisible);
    displayLayout->addWidget(m_titleVisible);
    displayLayout->addWidget(m_denseGrid);
    displayLayout->addWidget(m_syncXBtn);
    displayLayout->addWidget(m_resetViewBtn);
    displayLayout->addWidget(m_autoScaleBtn);
    displayLayout->addWidget(m_toggleLegendsBtn);
    m_saveTemplateBtn = new QPushButton(tr("Save Watch template…"), displayBox);
    m_loadTemplateBtn = new QPushButton(tr("Load Watch template…"), displayBox);
    displayLayout->addWidget(m_saveTemplateBtn);
    displayLayout->addWidget(m_loadTemplateBtn);
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
    auto* seriesActions = new QHBoxLayout();
    m_hideSerieBtn = new QPushButton(tr("Hide"), listBox);
    m_showSerieBtn = new QPushButton(tr("Show"), listBox);
    m_dupSerieBtn = new QPushButton(tr("Dup"), listBox);
    m_delSerieBtn = new QPushButton(tr("Delete"), listBox);
    m_upSerieBtn = new QPushButton(tr("↑"), listBox);
    m_downSerieBtn = new QPushButton(tr("↓"), listBox);
    seriesActions->addWidget(m_hideSerieBtn);
    seriesActions->addWidget(m_showSerieBtn);
    seriesActions->addWidget(m_dupSerieBtn);
    seriesActions->addWidget(m_delSerieBtn);
    seriesActions->addWidget(m_upSerieBtn);
    seriesActions->addWidget(m_downSerieBtn);
    listLayout->addLayout(seriesActions);
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
    m_gotoSourceBtn = new QPushButton(tr("Copy source path"), selectedBox);
    selectedForm->addRow(QString(), m_gotoSourceBtn);
    m_lineWidth = new QSpinBox(selectedBox);
    m_lineWidth->setRange(1, 8);
    m_lineStyle = new QComboBox(selectedBox);
    m_lineStyle->addItem(tr("Solid"), static_cast<int>(Qt::SolidLine));
    m_lineStyle->addItem(tr("Dash"), static_cast<int>(Qt::DashLine));
    m_lineStyle->addItem(tr("Dot"), static_cast<int>(Qt::DotLine));
    m_lineStyle->addItem(tr("Dash dot"), static_cast<int>(Qt::DashDotLine));
    selectedForm->addRow(tr("Line width"), m_lineWidth);
    selectedForm->addRow(tr("Line style"), m_lineStyle);

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
    m_quickAddBtn = new QPushButton(tr("Quick add Y(t)…"), m_seriesPage);
    seriesLayout->addWidget(m_addSerieBtn);
    seriesLayout->addWidget(m_quickAddBtn);
    connect(m_addSerieBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->createSelectionDialog(m_chartIndex);
        refreshFromTab();
        if (m_tab)
            m_tab->syncDocumentFromCharts();
    });
    connect(m_quickAddBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->openQuickAddDialog(m_chartIndex);
        refreshFromTab();
    });
    connect(m_seriesList, &QListWidget::currentRowChanged,
            this, &PlotSettingsSidePanel::onSeriesSelectionChanged);
    connect(m_hideSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        m_tab->pushSerieEnabledUndo(m_chartIndex, m_serieIndex, false);
        refreshFromTab();
    });
    connect(m_showSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        m_tab->pushSerieEnabledUndo(m_chartIndex, m_serieIndex, true);
        refreshFromTab();
    });
    connect(m_delSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        m_tab->pushSerieDeleteUndo(m_chartIndex, m_serieIndex);
        refreshFromTab();
    });
    connect(m_dupSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        m_tab->pushSerieDuplicateUndo(m_chartIndex, m_serieIndex);
        refreshFromTab();
    });
    connect(m_upSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        if (m_serieIndex > 0)
        {
            m_tab->pushSerieMoveUndo(m_chartIndex, m_serieIndex, m_serieIndex - 1);
            --m_serieIndex;
            refreshFromTab();
        }
    });
    connect(m_gotoSourceBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        UWatchChart* chart = m_tab->getChart(m_chartIndex);
        if (!chart) return;
        UWatchSerie* s = chart->getSerie(m_serieIndex);
        if (!s) return;
        const QString path = s->nameComponent + QLatin1Char('.') + s->nameProperty;
        QApplication::clipboard()->setText(path);
        m_bindingLabel->setToolTip(tr("Copied: %1").arg(path));
    });
    connect(m_lineWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &PlotSettingsSidePanel::applySeriesLive);
    connect(m_lineStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlotSettingsSidePanel::applySeriesLive);
    connect(m_downSerieBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab) return;
        if (auto* c = m_tab->getChart(m_chartIndex)) {
            if (m_serieIndex >= 0 && m_serieIndex + 1 < c->countSeries()) {
                m_tab->pushSerieMoveUndo(m_chartIndex, m_serieIndex, m_serieIndex + 1);
                ++m_serieIndex;
                refreshFromTab();
            }
        }
    });
    connect(m_panelUpBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab || m_chartIndex <= 0)
            return;
        m_tab->movePanel(m_chartIndex, m_chartIndex - 1);
        m_chartIndex = qMax(0, m_chartIndex - 1);
        refreshFromTab();
    });
    connect(m_panelDownBtn, &QPushButton::clicked, this, [this]() {
        if (!m_tab || m_chartIndex + 1 >= m_tab->countGraphs())
            return;
        m_tab->movePanel(m_chartIndex, m_chartIndex + 1);
        ++m_chartIndex;
        refreshFromTab();
    });
    connect(m_saveTemplateBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->saveWatchTemplateDialog();
    });
    connect(m_loadTemplateBtn, &QPushButton::clicked, this, [this]() {
        if (m_tab)
            m_tab->loadWatchTemplateDialog();
        refreshFromTab();
    });

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
    if (m_fixedYRange)
        connect(m_fixedYRange, &QCheckBox::toggled, this, [this, chartLive]() {
            updateAxesModeVisibility();
            chartLive();
        });
    if (m_fixedXRange)
        connect(m_fixedXRange, &QCheckBox::toggled, this, [this, chartLive]() {
            updateAxesModeVisibility();
            chartLive();
        });
    if (m_denseGrid)
        connect(m_denseGrid, &QCheckBox::toggled, this, [this](bool on) {
            if (m_refreshing || !m_tab)
                return;
            m_tab->setDenseMode(on);
        });
    if (m_syncXBtn)
        connect(m_syncXBtn, &QPushButton::clicked, this, [this]() {
            if (m_tab)
                m_tab->syncTimeSeriesXRangeFromActive();
        });
    if (m_resetViewBtn)
        connect(m_resetViewBtn, &QPushButton::clicked, this, [this]() {
            if (!m_tab) return;
            if (UWatchChart* c = m_tab->getChart(m_chartIndex))
                c->resetViewport();
        });
    if (m_autoScaleBtn)
        connect(m_autoScaleBtn, &QPushButton::clicked, this, [this]() {
            if (!m_tab) return;
            if (UWatchChart* c = m_tab->getChart(m_chartIndex))
            {
                c->restoreInitialAxesState();
                m_tab->syncDocumentFromCharts();
                refreshFromTab();
            }
        });
    if (m_toggleLegendsBtn)
        connect(m_toggleLegendsBtn, &QPushButton::clicked, this, [this]() {
            if (!m_tab) return;
            bool anyHidden = false;
            for (int i = 0; i < m_tab->countGraphs(); ++i)
            {
                if (UWatchChart* c = m_tab->getChart(i))
                {
                    if (!c->isLegendVisible())
                        anyHidden = true;
                }
            }
            m_tab->setAllLegendsVisible(anyHidden);
        });

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
        if (m_panelCombo)
        {
            QSignalBlocker b(m_panelCombo);
            m_panelCombo->clear();
        }
        return;
    }
    UWatchChart* chart = m_tab->getChart(m_chartIndex);
    const QString title = chart ? chart->getChartTitle() : QString();
    m_heroTitle->setText(tr("Watch / Tab / %1")
                             .arg(title.isEmpty() ? tr("Chart %1").arg(m_chartIndex + 1) : title));
    if (m_panelCombo)
    {
        QSignalBlocker b(m_panelCombo);
        m_panelCombo->clear();
        for (int i = 0; i < m_tab->countGraphs(); ++i)
        {
            UWatchChart* c = m_tab->getChart(i);
            QString label = c ? c->getChartTitle() : QString();
            if (label.isEmpty())
                label = tr("Chart %1").arg(i + 1);
            if (c && !c->isPanelVisible())
                label += tr(" [hidden]");
            label += tr(" · %1 series").arg(c ? c->countSeries() : 0);
            m_panelCombo->addItem(label, i);
        }
        const int idx = m_panelCombo->findData(m_chartIndex);
        if (idx >= 0)
            m_panelCombo->setCurrentIndex(idx);
    }
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
    const bool fixedY = m_fixedYRange && m_fixedYRange->isChecked();
    const bool fixedX = m_fixedXRange && m_fixedXRange->isChecked();
    if (m_fixedXRange)
        m_fixedXRange->setVisible(xy);
    if (m_xMin)
    {
        m_xMin->setVisible(xy);
        m_xMin->setEnabled(xy && fixedX);
    }
    if (m_xMax)
    {
        m_xMax->setVisible(xy);
        m_xMax->setEnabled(xy && fixedX);
    }
    if (m_yMin)
        m_yMin->setEnabled(fixedY);
    if (m_yMax)
        m_yMax->setEnabled(fixedY);
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
        if (m_fixedYRange)
            m_fixedYRange->setChecked(chart->isFixedYRange());
        if (m_fixedXRange)
            m_fixedXRange->setChecked(chart->isFixedXRange());
        if (m_denseGrid)
            m_denseGrid->setChecked(m_tab->isDenseMode());

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
            QString label = s ? s->name() : QStringLiteral("serie_%1").arg(i);
            if (s && !s->isVisible())
                label += tr(" [hidden]");
            if (s && !s->isOnline)
                label += tr(" [offline]");
            m_seriesList->addItem(label);
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
    if (m_lineWidth)
    {
        QSignalBlocker bw(m_lineWidth);
        m_lineWidth->setValue(chart->getSerieWidth(m_serieIndex));
    }
    if (m_lineStyle)
    {
        QSignalBlocker bs(m_lineStyle);
        const int style = static_cast<int>(chart->getSerieLineType(m_serieIndex));
        const int idx = m_lineStyle->findData(style);
        if (idx >= 0)
            m_lineStyle->setCurrentIndex(idx);
    }
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
    if (m_fixedYRange)
        chart->setFixedYRange(m_fixedYRange->isChecked());
    if (m_fixedXRange)
        chart->setFixedXRange(m_fixedXRange->isChecked());
    if (m_yMin->value() >= m_yMax->value())
    {
        QSignalBlocker b1(m_yMin);
        QSignalBlocker b2(m_yMax);
        m_yMin->setValue(chart->getAxisYmin());
        m_yMax->setValue(chart->getAxisYmax());
        return;
    }
    if (!m_fixedYRange || m_fixedYRange->isChecked())
    {
        chart->setAxisYmin(m_yMin->value());
        chart->setAxisYmax(m_yMax->value());
    }
    const auto viz = static_cast<NMSDK::Plot::VizKind>(m_vizKind->currentData().toInt());
    if (chart->countSeries() > 0 && !NMSDK::Plot::sameVizFamily(chart->getVizKind(), viz))
    {
        QSignalBlocker b(m_vizKind);
        const int idx = m_vizKind->findData(static_cast<int>(chart->getVizKind()));
        if (idx >= 0)
            m_vizKind->setCurrentIndex(idx);
        updateAxesModeVisibility();
        if (m_heroTitle)
            m_heroTitle->setToolTip(
                tr("Cannot switch Time series ↔ Y(x) while series exist. Clear series or use another chart."));
        return;
    }
    const bool xy = NMSDK::Plot::isXYFamily(viz);
    if (xy)
    {
        if (m_fixedXRange && m_fixedXRange->isChecked())
        {
            if (m_xMin->value() >= m_xMax->value())
                return;
            chart->setAxisXmin(m_xMin->value());
            chart->setAxisXmax(m_xMax->value());
        }
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
    chart->setSerieYshift(m_serieIndex, m_yShift->value());
    if (m_lineWidth)
        chart->setSerieWidth(m_serieIndex, m_lineWidth->value());
    if (m_lineStyle)
        chart->setSerieLineType(m_serieIndex,
                                static_cast<Qt::PenStyle>(m_lineStyle->currentData().toInt()));
    m_tab->syncDocumentFromCharts();
    if (m_serieIndex >= 0 && m_serieIndex < m_seriesList->count())
    {
        QSignalBlocker blocker(m_seriesList);
        m_seriesList->item(m_serieIndex)->setText(m_serieName->text());
    }
    emit requestApply();
}

bool PlotSettingsSidePanel::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_panelCombo && event->type() == QEvent::MouseButtonDblClick)
    {
        if (m_tab)
            m_tab->toggleExpandChart(m_chartIndex);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
