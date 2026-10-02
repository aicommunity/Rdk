#include "UWatchTab.h"
#include "ui_UWatchTab.h"
#include "UGuiTelemetry.h"
#include "Plot/PlotDataAdapter.h"
#include "Plot/PlotSettingsSidePanel.h"
#include "Plot/PlotSurface.h"
#include "Plot/UWatchLayoutDialog.h"
#include "Plot/UWatchSeriesWizard.h"
#include "Plot/UWatchQuickAddDialog.h"
#include "Plot/PlotWatchUndoCommands.h"
#include "Plot/WatchPropertyDndPayload.h"
#include "Plot/WatchTemplateStore.h"
#include "Plot/WatchDebug.h"
#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/Application/UApplication.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include <QShortcut>
#include <QKeySequence>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QUndoStack>
#include <QToolBar>
#include <QtGlobal>
#include <cstdio>



UWatchTab::UWatchTab(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UWatchTab)
{
    ui->setupUi(this);
    colSplitter = nullptr;
    m_undoStack = new QUndoStack(this);

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    chartsColumn = new QWidget(mainSplitter);
    auto* columnLayout = new QVBoxLayout(chartsColumn);
    columnLayout->setContentsMargins(0, 0, 0, 0);
    columnLayout->setSpacing(0);
    chartsHost = new QWidget(chartsColumn);
    auto* chartsLayout = new QHBoxLayout(chartsHost);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    chartsLayout->setSpacing(0);
    columnLayout->addWidget(chartsHost, 1);
    ensureSharedModeBar();
    mainSplitter->addWidget(chartsColumn);
    ui->horizontalLayout->addWidget(mainSplitter);

    createGridLayout(1,1);
    ensureSettingsPanel();
    syncDocumentFromCharts();
    setActiveChart(0);

    UpdateInterval = UpdateIntervalMs;
    setAccessibleName("UWatchTab");

    auto* escCollapse = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    escCollapse->setContext(Qt::WidgetWithChildrenShortcut);
    connect(escCollapse, &QShortcut::activated, this, [this]() {
        if (m_expandedIndex >= 0)
            collapseExpandedChart();
    });
    auto* undoShortcut = new QShortcut(QKeySequence::Undo, this);
    undoShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(undoShortcut, &QShortcut::activated, m_undoStack, &QUndoStack::undo);
    auto* redoShortcut = new QShortcut(QKeySequence::Redo, this);
    redoShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(redoShortcut, &QShortcut::activated, m_undoStack, &QUndoStack::redo);
}

UWatchTab::~UWatchTab()
{
    delete ui;
}

void UWatchTab::createGraph()
{
    graph.push_back(new NMSDK::Plot::PlotSurface(this));
    graph.last()->setChartIndex(graph.count()-1);
    graph.last()->ensurePanelId();

    connect(graph.last(), SIGNAL(addSerieSignal(int)), this, SLOT(createSelectionDialogSlot(int)));
    connect(graph.last(), SIGNAL(openSettingsPanel(int,bool)), this, SLOT(openSettingsPanelSlot(int,bool)));
    connect(graph.last(), SIGNAL(chartActivated(int)), this, SLOT(onChartActivated(int)));
    connect(graph.last(), &UWatchChart::expandToggleRequested, this, &UWatchTab::onExpandToggleRequested);
    connect(graph.last(), &UWatchChart::saveChartAsRequested, this, &UWatchTab::onSaveChartAsRequested);
    connect(graph.last(), &UWatchChart::quickSaveChartRequested, this, &UWatchTab::onQuickSaveChartRequested);
    connect(graph.last(), &UWatchChart::duplicatePanelRequested, this, &UWatchTab::onDuplicatePanelRequested);
    connect(graph.last(), &UWatchChart::hidePanelRequested, this, &UWatchTab::onHidePanelRequested);
    connect(graph.last(), &UWatchChart::deletePanelRequested, this, &UWatchTab::onDeletePanelRequested);
}

void UWatchTab::deleteGraph(int index)
{
    delete graph[index];
    graph.remove(index);
}


void UWatchTab::AUpdateInterface()
{
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UWatchTab"), accessibleName());

    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (!env) {
        return;
    }

    for (int graphIndex = 0; graphIndex < graph.count(); graphIndex++) {
        if (graph[graphIndex] && graph[graphIndex]->chartView) {
            graph[graphIndex]->chartView->setUpdatesEnabled(false);
        }
    }

    for (int graphIndex = 0; graphIndex < graph.count(); graphIndex++)
    {
        if (!graph[graphIndex]) {
            continue;
        }

        double x_min = 0.0;
        double x_max = 0.0;
        bool hadXSamples = false;
        const NMSDK::Plot::VizKind panelViz = graph[graphIndex]->getVizKind();
        const bool panelIsXY = NMSDK::Plot::isXYFamily(panelViz);

        int i = 0;
        while (i < graph[graphIndex]->countSeries())
        {
            UWatchSerie* serie = graph[graphIndex]->getSerie(i);
            if (serie && serie->isMatrixSliceXY())
            {
                ++i;
                continue;
            }
            RDK::UControllerDataReader* data_reader = env->GetDataReader(
                serie->nameComponent.toStdString(),
                serie->nameProperty.toStdString(),
                serie->Jx < 0 ? 0 : serie->Jx,
                serie->Jy < 0 ? 0 : serie->Jy);
            bool xOk = true;
            if (serie->vizKind == NMSDK::Plot::VizKind::XYLine
                || serie->vizKind == NMSDK::Plot::VizKind::XYScatter)
            {
                xOk = env->GetDataReader(
                          serie->xNameComponent.toStdString(),
                          serie->xNameProperty.toStdString(),
                          serie->xJx < 0 ? 0 : serie->xJx,
                          serie->xJy < 0 ? 0 : serie->xJy)
                      != nullptr;
            }
            if (!data_reader || !xOk) {
                // Offline instead of dropping entire series on transient reader gaps.
                if (serie)
                    serie->setOnlineStatus(false);
                ++i;
            } else {
                if (serie)
                    serie->setOnlineStatus(true);
                ++i;
            }
        }

        for (int serieIndex = 0; serieIndex < graph[graphIndex]->countSeries(); serieIndex++)
        {
            UWatchSerie *current_serie = graph[graphIndex]->getSerie(serieIndex);
            if (!current_serie)
                continue;

            const bool isXY = NMSDK::Plot::isXYFamily(current_serie->vizKind);
            NMSDK::Plot::PlotSeries dto = current_serie->toPlotSeries();
            QVector<QPointF> samplePoints;

            if (current_serie->isMatrixSliceXY())
            {
                samplePoints = NMSDK::Plot::sampleMatrixSlicePair(dto, current_serie->YShift);
                current_serie->setOnlineStatus(!samplePoints.isEmpty()
                                               || current_serie->isOnline);
            }
            else if (!current_serie->isOnline)
            {
                continue;
            }
            else if (isXY && dto.binding.x.kind == NMSDK::Plot::DataRoleKind::Property)
            {
                samplePoints = NMSDK::Plot::samplePropertyPair(
                    env.Get(),
                    dto,
                    current_serie->YShift,
                    current_serie->xyRing,
                    current_serie->xyLastXSimTime,
                    current_serie->xyLastYSimTime,
                    current_serie->xyLastAcceptSimTime);
            }
            else
            {
                samplePoints = NMSDK::Plot::sampleTimeSeries(
                    env.Get(), dto, current_serie->YShift);
            }

            if (!current_serie->isMatrixSliceXY())
            {
                RDK::UControllerDataReader* data_reader = env->GetDataReader(
                    current_serie->nameComponent.toStdString(),
                    current_serie->nameProperty.toStdString(),
                    current_serie->Jx < 0 ? 0 : current_serie->Jx,
                    current_serie->Jy < 0 ? 0 : current_serie->Jy);
                current_serie->setOnlineStatus(data_reader != nullptr);
            }
            else
            {
                current_serie->setOnlineStatus(!samplePoints.isEmpty());
            }

            if (!samplePoints.isEmpty())
            {
                // XY is sorted by X — never incremental-append (new point may land mid-series).
                if (isXY)
                {
                    current_serie->replace(samplePoints);
                }
                else
                {
                    // Incremental append when only one new point; else full replace.
                    const int oldCount = current_serie->count();
                    const int newCount = samplePoints.size();
                    if (oldCount > 0 && newCount == oldCount + 1
                        && current_serie->at(oldCount - 1) == samplePoints.at(oldCount - 1))
                    {
                        current_serie->append(samplePoints.last());
                    }
                    else
                    {
                        const int decimationThreshold = 8000;
                        if (newCount > decimationThreshold)
                            current_serie->replace(
                                NMSDK::Plot::decimatePointsEnvelope(samplePoints, decimationThreshold));
                        else
                        {
                            current_serie->replace(samplePoints);
                        }
                    }
                }

                if (!isXY)
                {
                    if (!hadXSamples || x_min > samplePoints.first().x())
                        x_min = samplePoints.first().x();
                    if (!hadXSamples || x_max < samplePoints.last().x())
                        x_max = samplePoints.last().x();
                    hadXSamples = true;
                }
                else
                {
                    for (const QPointF& pt : samplePoints)
                    {
                        if (!hadXSamples || x_min > pt.x())
                            x_min = pt.x();
                        if (!hadXSamples || x_max < pt.x())
                            x_max = pt.x();
                        hadXSamples = true;
                    }
                }
            }
        }

        const bool zoomed = graph[graphIndex]->checkZoomed();
        const bool trackable = graph[graphIndex]->getIsAxisXtrackable();
        double W = graph[graphIndex]->getAxisXrange();
        double lo = x_min;
        double hi = x_max;
        bool trackApplied = false;

        if (!zoomed && hadXSamples)
        {
            if (!panelIsXY)
            {
                if (trackable)
                {
                    // Single W = axisXrange = reader.TimeInterval.
                    // Pre-refactor pad + clamp when span > W (keeps newest visible).
                    if (W > 0.0)
                    {
                        if (hi - lo < W)
                            hi = lo + W;
                        if (hi - lo > W)
                            lo = hi - W;
                    }
                    graph[graphIndex]->setAxisXRange(lo, hi);
                    graph[graphIndex]->fixInitialAxesState();
                    trackApplied = true;
                }
            }
            else
            {
                // XY: follow sample extents unless Fixed X range is enabled.
                if (!graph[graphIndex]->isFixedXRange())
                {
                    if (!(x_max > x_min))
                    {
                        const double pad = qMax(0.5, qAbs(x_min) * 0.01);
                        x_min -= pad;
                        x_max += pad;
                    }
                    lo = x_min;
                    hi = x_max;
                    graph[graphIndex]->setAxisXRange(x_min, x_max);
                    graph[graphIndex]->fixInitialAxesState();
                    trackApplied = true;
                }
            }
        }

        if (NMSDK::WatchDebug::enabled() && !panelIsXY)
        {
            static QElapsedTimer s_watchDbgTimer;
            static bool s_watchDbgTimerInit = false;
            if (!s_watchDbgTimerInit)
            {
                s_watchDbgTimer.start();
                s_watchDbgTimerInit = true;
            }
            // ~1 Hz across all charts (first chart that hits the gate logs).
            if (graphIndex == 0 && s_watchDbgTimer.elapsed() >= 1000)
            {
                s_watchDbgTimer.restart();
                double rTi = -1.0;
                int rNp = -1;
                int rN = -1;
                double rFront = 0.0;
                double rBack = 0.0;
                if (graph[graphIndex]->countSeries() > 0)
                {
                    UWatchSerie* s0 = graph[graphIndex]->getSerie(0);
                    RDK::UControllerDataReader* rd = s0 ? s0->data_reader : nullptr;
                    if (!rd && s0 && env)
                    {
                        rd = env->GetDataReader(
                            s0->nameComponent.toStdString(),
                            s0->nameProperty.toStdString(),
                            s0->Jx < 0 ? 0 : s0->Jx,
                            s0->Jy < 0 ? 0 : s0->Jy);
                    }
                    if (rd)
                    {
                        rTi = rd->TimeInterval;
                        rNp = rd->NumPoints;
                        rN = static_cast<int>(rd->XData.size());
                        if (!rd->XData.empty())
                        {
                            rFront = rd->XData.front();
                            rBack = rd->XData.back();
                        }
                    }
                }
                const double axisLo = graph[graphIndex]->getAxisXmin();
                const double axisHi = graph[graphIndex]->getAxisXmax();
                const double sampleSpan = hadXSamples ? (x_max - x_min) : -1.0;
                std::fprintf(stderr,
                    "[WatchDebug] g=%d W=%.4f sampleSpan=%.4f x=[%.4f,%.4f] "
                    "lohi=[%.4f,%.4f] axis=[%.4f,%.4f] axisW=%.4f "
                    "zoomed=%d trackable=%d applied=%d "
                    "readerTi=%.4f np=%d n=%d rX=[%.4f,%.4f] rSpan=%.4f\n",
                    graphIndex, W, sampleSpan, x_min, x_max, lo, hi,
                    axisLo, axisHi, axisHi - axisLo,
                    zoomed ? 1 : 0, trackable ? 1 : 0, trackApplied ? 1 : 0,
                    rTi, rNp, rN, rFront, rBack,
                    (rN > 0) ? (rBack - rFront) : -1.0);
                std::fflush(stderr);
            }
        }
    }

    for (int graphIndex = 0; graphIndex < graph.count(); graphIndex++) {
        if (graph[graphIndex] && graph[graphIndex]->chartView) {
            graph[graphIndex]->chartView->setUpdatesEnabled(true);
            graph[graphIndex]->commitUpdate();
        }
    }
}


///Очищает интерфейс
void UWatchTab::AClearInterface()
{
 int count=graph.count();
 for(int i=count-1;i>=0;i--)
  deleteGraph(i);
}


/// Безопасно считывает данные серии из ядра
/// @deprecated Используется только для обратной совместимости.
/// В AUpdateInterface теперь используется прямой доступ с единой блокировкой.
void UWatchTab::ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata)
{
    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (!env) {
        xdata.clear();
        ydata.clear();
        return;
    }

    UWatchSerie *serie = graph[graphIndex]->getSerie(serieIndex);
    if (!serie) {
        xdata.clear();
        ydata.clear();
        return;
    }

    RDK::UControllerDataReader* data_reader = env->GetDataReader(
        serie->nameComponent.toStdString(),
        serie->nameProperty.toStdString(),
        serie->Jx,
        serie->Jy);

    if (!data_reader)
    {
        xdata.clear();
        ydata.clear();
        return;
    }

    xdata = data_reader->XData;
    ydata = data_reader->YData;
}

void UWatchTab::createSelectionDialogSlot(int index)
{
    createSelectionDialog(index);
}

void UWatchTab::setActiveChart(int index)
{
    if (graph.isEmpty())
    {
        m_activeChartIndex = 0;
        return;
    }
    if (index < 0)
        index = 0;
    if (index >= graph.count())
        index = graph.count() - 1;
    m_activeChartIndex = index;
    for (int i = 0; i < graph.count(); ++i)
    {
        if (graph[i])
            graph[i]->setSelected(i == m_activeChartIndex);
    }
    if (settingsPanel && settingsPanel->isVisible())
        settingsPanel->setActiveChart(m_activeChartIndex);
    syncSharedModeBarFromActive();
}

void UWatchTab::onChartActivated(int chartIndex)
{
    setActiveChart(chartIndex);
}

void UWatchTab::updateExpandActionsVisibility()
{
    const bool multi = countGraphs() > 1;
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        graph[i]->setExpandActionVisible(multi);
        graph[i]->setExpandChecked(multi && i == m_expandedIndex);
    }
}

void UWatchTab::restoreExpandedSplitterSizes()
{
    if (colSplitter && !m_savedColSizes.isEmpty()
        && m_savedColSizes.size() == colSplitter->count())
    {
        colSplitter->setSizes(m_savedColSizes);
    }
    for (int r = 0; r < rowSplitter.size() && r < m_savedRowSizes.size(); ++r)
    {
        if (rowSplitter[r] && !m_savedRowSizes[r].isEmpty()
            && m_savedRowSizes[r].size() == rowSplitter[r]->count())
        {
            rowSplitter[r]->setSizes(m_savedRowSizes[r]);
        }
    }
}

void UWatchTab::collapseExpandedChart()
{
    if (m_expandedIndex < 0)
        return;
    m_expandedIndex = -1;
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        graph[i]->show();
        graph[i]->setExpandChecked(false);
    }
    restoreExpandedSplitterSizes();
    m_savedColSizes.clear();
    m_savedRowSizes.clear();
    updateExpandActionsVisibility();
}

void UWatchTab::toggleExpandChart(int chartIndex)
{
    if (countGraphs() <= 1)
        return;
    if (chartIndex < 0 || chartIndex >= graph.count() || !graph[chartIndex])
        return;

    if (m_expandedIndex == chartIndex)
    {
        collapseExpandedChart();
        return;
    }

    if (m_expandedIndex < 0)
    {
        m_savedColSizes = captureColSplitterSizes();
        m_savedRowSizes = captureRowSplitterSizes();
    }

    m_expandedIndex = chartIndex;
    setActiveChart(chartIndex);
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        const bool show = (i == chartIndex);
        graph[i]->setVisible(show);
        graph[i]->setExpandChecked(show);
    }
    updateExpandActionsVisibility();
}

void UWatchTab::onExpandToggleRequested(int chartIndex)
{
    toggleExpandChart(chartIndex);
}

QString UWatchTab::savedWatchesRoot() const
{
    if (!application)
        return {};
    QString path = QString::fromStdString(application->GetProjectPath());
    if (path.isEmpty())
        return {};
    if (!path.endsWith(QLatin1Char('/')) && !path.endsWith(QLatin1Char('\\')))
        path += QLatin1Char('/');
    return path + QStringLiteral("SavedWatches/");
}

bool UWatchTab::exportChartToPath(int chartIndex, const QString& path)
{
    if (chartIndex < 0 || chartIndex >= graph.count() || !graph[chartIndex])
        return false;
    return graph[chartIndex]->exportImage(path);
}

int UWatchTab::exportAllChartsToDirectory(const QString& dirPath, const QString& extension)
{
    QDir dir(dirPath);
    if (!dir.exists() && !dir.mkpath(QStringLiteral(".")))
        return 0;
    const QString ext = extension.startsWith(QLatin1Char('.'))
                            ? extension.mid(1)
                            : extension;
    int saved = 0;
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        const QString name = QStringLiteral("%1_%2.%3")
                                 .arg(i + 1, 2, 10, QLatin1Char('0'))
                                 .arg(graph[i]->sanitizedTitleForFile())
                                 .arg(ext);
        if (graph[i]->exportImage(dir.filePath(name)))
            ++saved;
    }
    return saved;
}

void UWatchTab::onSaveChartAsRequested(int chartIndex)
{
    setActiveChart(chartIndex);
    QString startDir = savedWatchesRoot();
    if (startDir.isEmpty())
    {
        QMessageBox::warning(this, tr("Save chart"),
                             tr("Open a project first so charts can be saved under the configuration folder."));
        return;
    }
    QDir().mkpath(startDir);
    UWatchChart* chart = getChart(chartIndex);
    if (!chart)
        return;
    const QString defaultName = startDir + chart->sanitizedTitleForFile() + QStringLiteral(".png");
    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Save chart"),
        defaultName,
        tr("PNG (*.png);;SVG (*.svg);;JPEG (*.jpg *.jpeg)"));
    if (path.isEmpty())
        return;
    if (!exportChartToPath(chartIndex, path))
    {
        QMessageBox::warning(this, tr("Save chart"), tr("Failed to save chart."));
        return;
    }
}

void UWatchTab::onQuickSaveChartRequested(int chartIndex)
{
    if (!quickSaveOneChart(chartIndex))
    {
        // Errors already reported inside when project path missing / IO fail.
    }
}

QString UWatchTab::ensureQuickSaveSessionDir()
{
    if (!m_quickSaveDir.isEmpty() && QDir(m_quickSaveDir).exists())
        return m_quickSaveDir;

    const QString root = savedWatchesRoot();
    if (root.isEmpty())
    {
        QMessageBox::warning(this, tr("Quick save"),
                             tr("Open a project first so charts can be saved under SavedWatches."));
        return {};
    }
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_HH-mm-ss"));
    const QString dir = root + stamp + QLatin1Char('/');
    if (!QDir().mkpath(dir))
    {
        QMessageBox::warning(this, tr("Quick save"), tr("Cannot create folder:\n%1").arg(dir));
        return {};
    }
    m_quickSaveDir = dir;
    return m_quickSaveDir;
}

int UWatchTab::quickSaveAllCharts()
{
    const QString dir = ensureQuickSaveSessionDir();
    if (dir.isEmpty())
        return 0;
    const QString tick = QDateTime::currentDateTime().toString(QStringLiteral("HH-mm-ss"));
    int saved = 0;
    if (countGraphs() == 1)
    {
        UWatchChart* chart = getChart(0);
        if (!chart)
            return 0;
        const QString path = dir + QStringLiteral("chart_%1.png").arg(tick);
        if (exportChartToPath(0, path))
            ++saved;
        return saved;
    }
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        const QString path = dir
                             + QStringLiteral("%1_%2_%3.png")
                                   .arg(i + 1, 2, 10, QLatin1Char('0'))
                                   .arg(graph[i]->sanitizedTitleForFile())
                                   .arg(tick);
        if (exportChartToPath(i, path))
            ++saved;
    }
    return saved;
}

bool UWatchTab::quickSaveOneChart(int chartIndex)
{
    const QString dir = ensureQuickSaveSessionDir();
    if (dir.isEmpty())
        return false;
    UWatchChart* chart = getChart(chartIndex);
    if (!chart)
        return false;
    const QString tick = QDateTime::currentDateTime().toString(QStringLiteral("HH-mm-ss"));
    const QString path = dir
                         + QStringLiteral("%1_%2_%3.png")
                               .arg(chartIndex + 1, 2, 10, QLatin1Char('0'))
                               .arg(chart->sanitizedTitleForFile())
                               .arg(tick);
    if (!exportChartToPath(chartIndex, path))
    {
        QMessageBox::warning(this, tr("Quick save"), tr("Failed to save chart."));
        return false;
    }
    return true;
}

void UWatchTab::showInspector(PlotInspectorPage page, int chartIndex)
{
    ensureSettingsPanel();
    if (!settingsPanel)
        return;
    if (chartIndex < 0)
        chartIndex = m_activeChartIndex;
    setActiveChart(chartIndex);
    settingsPanel->showInspector(page, m_activeChartIndex);
    updateInspectorSplitterSizes(true);
}

void UWatchTab::hideInspector()
{
    if (!settingsPanel)
        return;
    settingsPanel->hide();
    updateInspectorSplitterSizes(false);
}

bool UWatchTab::isInspectorVisible() const
{
    return settingsPanel && settingsPanel->isVisible();
}

void UWatchTab::updateInspectorSplitterSizes(bool show)
{
    if (!mainSplitter || !settingsPanel)
        return;
    if (show)
    {
        settingsPanel->show();
        const int total = qMax(mainSplitter->width(), 400);
        const int panelW = 360;
        mainSplitter->setSizes({qMax(1, total - panelW), panelW});
    }
    else
    {
        settingsPanel->hide();
        mainSplitter->setSizes({1, 0});
    }
}

void UWatchTab::layoutOptionTriggered()
{
    UWatchLayoutDialog::execForTab(this, this);
}

void UWatchTab::seriesOptionTriggered()
{
    showInspector(PlotInspectorPage::Series, m_activeChartIndex);
}

void UWatchTab::chartsOptionTriggered()
{
    showInspector(PlotInspectorPage::Chart, m_activeChartIndex);
}

void UWatchTab::openSettingsPanelSlot(int chartIndex, bool seriesPage)
{
    showInspector(seriesPage ? PlotInspectorPage::Series : PlotInspectorPage::Chart, chartIndex);
}

void UWatchTab::ensureSettingsPanel()
{
    if (settingsPanel)
        return;
    if (!mainSplitter)
        return;
    settingsPanel = new PlotSettingsSidePanel(this, mainSplitter);
    mainSplitter->addWidget(settingsPanel);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 0);
    mainSplitter->setCollapsible(1, true);
    connect(settingsPanel, &PlotSettingsSidePanel::requestHide, this, &UWatchTab::hideInspector);
    settingsPanel->hide();
    updateInspectorSplitterSizes(false);
}

void UWatchTab::createSplitterGrid(int rowNumber)
{
    colSplitter = new QSplitter(chartsHost ? chartsHost : this);
    colSplitter->setOrientation(Qt::Vertical);

    QList<int> sizes;
    int height = this->height();
    for (int i = 0; i < rowNumber; ++i)
    {
        rowSplitter.push_back(new QSplitter(this));
        rowSplitter.last()->setOrientation(Qt::Horizontal);
        colSplitter->addWidget(rowSplitter.last());
        sizes.push_back(height/rowNumber);
    }
    colSplitter->setSizes(sizes);
    if (chartsHost && chartsHost->layout())
        chartsHost->layout()->addWidget(colSplitter);
    else
        ui->horizontalLayout->addWidget(colSplitter);
}

void UWatchTab::tearDownSplitterLayout()
{
    // Detach chart widgets from splitters without destroying panels/series.
    for (int i = tabRowNumber - 1; i >= 0; --i)
    {
        if (i >= rowSplitter.size() || !rowSplitter[i])
            continue;
        while (rowSplitter[i]->count() > 0)
        {
            QWidget* w = rowSplitter[i]->widget(0);
            if (w)
                w->setParent(nullptr);
        }
        delete rowSplitter.takeLast();
    }
    tabRowNumber = 0;

    if (colSplitter != nullptr)
    {
        if (chartsHost && chartsHost->layout())
            chartsHost->layout()->removeWidget(colSplitter);
        else
            ui->horizontalLayout->removeWidget(colSplitter);
        delete colSplitter;
        colSplitter = nullptr;
    }
}

void UWatchTab::deleteGraphs(int new_graph_count)
{
    // Legacy helper: used only when explicitly shrinking the document composition.
    // Layout changes must call createGridLayout (non-destructive) instead.
    int graphs_to_remove = countGraphs() - new_graph_count;
    if (graphs_to_remove < 0)
        graphs_to_remove = 0;

    tearDownSplitterLayout();

    for (int i = 0; i < graphs_to_remove; ++i)
    {
        if (graph.isEmpty())
            break;
        delete graph.takeLast();
    }
}

void UWatchTab::createGridLayout(int rowNumber, int colNumber)
{
    collapseExpandedChart();

    if (rowNumber < 1)
        rowNumber = 1;
    if (colNumber < 1)
        colNumber = 1;

    // Layout ≠ composition: rebuild geometry only; never delete surplus panels.
    tearDownSplitterLayout();

    tabColNumber = colNumber;
    tabRowNumber = rowNumber;
    createSplitterGrid(rowNumber);

    const int gridSlots = tabColNumber * tabRowNumber;
    while (countGraphs() < gridSlots)
        createGraph();

    // Visible panels fill gridSlots first; overflow panels stay in the document but hidden.
    QVector<UWatchChart*> visibleCharts;
    QVector<UWatchChart*> overflowCharts;
    visibleCharts.reserve(graph.size());
    for (UWatchChart* c : graph)
    {
        if (!c)
            continue;
        if (c->isPanelVisible())
            visibleCharts.push_back(c);
        else
            overflowCharts.push_back(c);
    }

    // Revive hidden panels before creating new ones when grid grows.
    while (static_cast<int>(visibleCharts.size()) < gridSlots && !overflowCharts.isEmpty())
    {
        UWatchChart* c = overflowCharts.takeFirst();
        c->setPanelVisible(true);
        visibleCharts.push_back(c);
    }
    while (static_cast<int>(visibleCharts.size()) < gridSlots)
    {
        createGraph();
        UWatchChart* c = graph.last();
        c->setPanelVisible(true);
        visibleCharts.push_back(c);
    }

    // Charts beyond slot count stay hidden (not destroyed).
    for (int i = gridSlots; i < visibleCharts.size(); ++i)
    {
        visibleCharts[i]->setPanelVisible(false);
        visibleCharts[i]->hide();
        overflowCharts.push_back(visibleCharts[i]);
    }
    visibleCharts.resize(gridSlots);

    for (UWatchChart* c : overflowCharts)
    {
        if (c)
            c->hide();
    }

    const int width = qMax(this->width(), 100);
    int k = 0;
    for (int i = 0; i < rowNumber; ++i)
    {
        QList<int> sizes;
        for (int j = 0; j < colNumber; ++j)
        {
            UWatchChart* cell = visibleCharts[k];
            cell->show();
            cell->setPanelVisible(true);
            // Do not overwrite user titles on layout change.
            if (cell->getChartTitle().trimmed().isEmpty())
                cell->setChartTitle(tr("Chart %1").arg(k + 1));
            rowSplitter[i]->addWidget(cell);
            sizes.push_back(width / colNumber);
            ++k;
        }
        rowSplitter[i]->setSizes(sizes);
    }

    // Keep graph vector order: visible slot order, then overflow.
    graph.clear();
    for (UWatchChart* c : visibleCharts)
        graph.push_back(c);
    for (UWatchChart* c : overflowCharts)
    {
        if (!graph.contains(c))
            graph.push_back(c);
    }
    for (int i = 0; i < graph.size(); ++i)
    {
        if (graph[i])
            graph[i]->setChartIndex(i);
    }

    applyDenseMode(gridSlots > 1 || m_document.denseGrid);
    if (m_activeChartIndex < 0 || m_activeChartIndex >= gridSlots)
        m_activeChartIndex = 0;
    setActiveChart(m_activeChartIndex);
    updateExpandActionsVisibility();
    syncDocumentFromCharts();
    if (settingsPanel)
        settingsPanel->refreshFromTab();
}

UWatchChart *UWatchTab::getChart(int index)
{
    return graph[index];
}


int UWatchTab::countGraphs()
{
    return graph.count();
}

void UWatchTab::createSelectionDialog(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.count() || !graph[chartIndex])
        return;

    UWatchSeriesWizard wiz(graph[chartIndex], application, this);
    wiz.setWindowTitle(tr("Add series"));
    if (wiz.exec() != QDialog::Accepted)
        return;
    if (wiz.applyToChart(graph[chartIndex]) > 0)
        syncDocumentFromCharts();
}

void UWatchTab::saveUpdateInterval(int newInterval)
{
    UpdateIntervalMs = newInterval;
    UpdateInterval = newInterval;
}

void UWatchTab::updateTheme()
{
    // Применяем стили темы ко всем графикам
    for(int i = 0; i < graph.count(); i++)
    {
        if(graph[i])
        {
            graph[i]->applyTheme();
        }
    }
}

int UWatchTab::getColNumber()
{
    return tabColNumber;
}

int UWatchTab::getRowNumber()
{
    return tabRowNumber;
}


// Сохраняет параметры интерфейса в xml
void UWatchTab::ASaveParameters(RDK::USerStorageXML &xml)
{
    // CurrentNode уже выбран SaveParameters (узел вкладки). Нельзя вызывать
    // DelNodeInternalContent, если SelectNodeForce не удался и мы всё ещё на
    // Interfaces — это сносит всё дерево GUI.
    syncDocumentFromCharts();
    xml.DelNodeInternalContent();
    NMSDK::Plot::savePlotDocument(xml, m_document);
}

void UWatchTab::syncDocumentFromCharts()
{
    m_document = capturePlotDocument();
}

NMSDK::Plot::PlotDocument UWatchTab::capturePlotDocument() const
{
    NMSDK::Plot::PlotDocument doc;
    doc.schemaVersion = NMSDK::Plot::PlotDocument::CurrentSchemaVersion;
    doc.gridCols = tabColNumber;
    doc.gridRows = tabRowNumber;
    doc.denseGrid = m_denseMode;
    doc.colSplitterSizes = captureColSplitterSizes();
    doc.rowSplitterSizes = captureRowSplitterSizes();
    for (int i = 0; i < graph.count(); ++i)
    {
        if (!graph[i])
            continue;
        NMSDK::Plot::PlotPanel panel = graph[i]->toPlotPanel();
        panel.updateIntervalMs = UpdateIntervalMs;
        doc.panels.push_back(panel);
    }
    return doc;
}

QList<int> UWatchTab::captureColSplitterSizes() const
{
    if (!colSplitter)
        return {};
    return colSplitter->sizes();
}

QVector<QList<int>> UWatchTab::captureRowSplitterSizes() const
{
    QVector<QList<int>> out;
    out.reserve(rowSplitter.size());
    for (QSplitter* s : rowSplitter)
        out.push_back(s ? s->sizes() : QList<int>());
    return out;
}

void UWatchTab::applySplitterSizes(const NMSDK::Plot::PlotDocument& doc)
{
    if (colSplitter && !doc.colSplitterSizes.isEmpty()
        && doc.colSplitterSizes.size() == colSplitter->count())
    {
        colSplitter->setSizes(doc.colSplitterSizes);
    }
    for (int r = 0; r < rowSplitter.size() && r < doc.rowSplitterSizes.size(); ++r)
    {
        if (rowSplitter[r] && !doc.rowSplitterSizes[r].isEmpty()
            && doc.rowSplitterSizes[r].size() == rowSplitter[r]->count())
        {
            rowSplitter[r]->setSizes(doc.rowSplitterSizes[r]);
        }
    }
}

void UWatchTab::applyPlotDocument(const NMSDK::Plot::PlotDocument& doc)
{
    collapseExpandedChart();

    int rows = doc.gridRows > 0 ? doc.gridRows : 1;
    int cols = doc.gridCols > 0 ? doc.gridCols : 1;
    const int panelCount = doc.panels.size();
    // GraphCount и rows*cols могут расходиться в старых/битых XML — не бросаем
    // всю конфигурацию серий из-за раннего return.
    if (panelCount > 0 && rows * cols != panelCount)
    {
        if (cols < 1)
            cols = 1;
        rows = (panelCount + cols - 1) / cols;
        if (rows < 1)
            rows = 1;
    }

    createGridLayout(rows, cols);

    const int applyCount = qMin(panelCount, countGraphs());
    for (int graphIndex = 0; graphIndex < applyCount; ++graphIndex)
    {
        const NMSDK::Plot::PlotPanel& panel = doc.panels[graphIndex];
        UWatchChart* chart = graph[graphIndex];
        if (!chart)
            continue;

        while (chart->countSeries() > 0)
            chart->deleteSerie(0);

        chart->applyPlotPanelMeta(panel);
        if (panel.updateIntervalMs > 0)
            saveUpdateInterval(panel.updateIntervalMs);

        for (const NMSDK::Plot::PlotSeries& serie : panel.series)
        {
            const double time_interval = panel.axisXRange > 0 ? panel.axisXRange : chart->getAxisXrange();
            const bool serieIsXY = serie.binding.x.kind == NMSDK::Plot::DataRoleKind::Property;
            const NMSDK::Plot::VizKind wantViz = serieIsXY
                ? (NMSDK::Plot::isXYFamily(panel.viz) ? panel.viz : NMSDK::Plot::VizKind::XYLine)
                : NMSDK::Plot::VizKind::TimeSeries;
            if (!chart->canAddVizKind(wantViz))
                continue;
            if (serieIsXY)
            {
                chart->createSerieXY(
                    serie.binding.channel,
                    serie.binding.x.prop.component,
                    serie.binding.x.prop.property,
                    serie.binding.x.prop.jx,
                    serie.binding.x.prop.jy,
                    serie.binding.y.prop.component,
                    serie.binding.y.prop.property,
                    serie.binding.y.prop.jx,
                    serie.binding.y.prop.jy,
                    serie.yOffset,
                    wantViz,
                    serie.binding.x.prop.slice,
                    serie.binding.y.prop.slice);
            }
            else
            {
                chart->createSerie(
                    serie.binding.channel,
                    serie.binding.y.prop.component,
                    serie.binding.y.prop.property,
                    QString(),
                    serie.binding.y.prop.jx < 0 ? 0 : serie.binding.y.prop.jx,
                    serie.binding.y.prop.jy < 0 ? 0 : serie.binding.y.prop.jy,
                    time_interval,
                    serie.yOffset);
            }

            const int idx = chart->countSeries() - 1;
            if (idx < 0)
                continue;
            if (!serie.visual.displayName.isEmpty())
                chart->setSerieName(idx, serie.visual.displayName);
            chart->setSerieWidth(idx, serie.visual.width);
            chart->setSerieLineType(idx, static_cast<Qt::PenStyle>(serie.visual.penStyle));
            chart->getSerie(idx)->setColor(serie.visual.color);
            chart->getSerie(idx)->windowSize = serie.binding.windowSize;
            chart->getSerie(idx)->xyMinIntervalMs = serie.binding.xyMinIntervalMs;
            chart->getSerie(idx)->xyMinDistance = serie.binding.xyMinDistance;
            chart->setSerieEnabled(idx, serie.enabled);
        }
    }
    applySplitterSizes(doc);
    m_document = doc;
    m_document.schemaVersion = NMSDK::Plot::PlotDocument::CurrentSchemaVersion;
    applyDenseMode(doc.denseGrid || (doc.gridRows * doc.gridCols > 1));
    updateExpandActionsVisibility();
}

// Загружает параметры интерфейса из xml
void UWatchTab::ALoadParameters(RDK::USerStorageXML &xml)
{
    NMSDK::Plot::PlotDocument doc;
    if (!NMSDK::Plot::loadPlotDocument(xml, doc))
    {
        // Fallback empty
        createGridLayout(1, 1);
        return;
    }
    applyPlotDocument(doc);
}

void UWatchTab::setDenseMode(bool dense)
{
    applyDenseMode(dense);
    syncDocumentFromCharts();
}

void UWatchTab::applyDenseMode(bool dense)
{
    m_denseMode = dense;
    m_document.denseGrid = dense;
    for (UWatchChart* c : graph)
    {
        if (c)
            c->setDenseChrome(dense);
    }
    if (m_sharedModeBar)
        m_sharedModeBar->setVisible(dense && countGraphs() > 0);
    syncSharedModeBarFromActive();
}

void UWatchTab::ensureSharedModeBar()
{
    if (m_sharedModeBar || !chartsColumn)
        return;
    auto* columnLayout = qobject_cast<QVBoxLayout*>(chartsColumn->layout());
    if (!columnLayout)
        return;

    m_sharedModeBar = new QToolBar(tr("Active chart tools"), chartsColumn);
    m_sharedModeBar->setObjectName(QStringLiteral("watchSharedModeBar"));
    m_sharedModeBar->setIconSize(QSize(16, 16));
    m_sharedModeBar->setMovable(false);
    m_sharedPan = m_sharedModeBar->addAction(tr("Pan"));
    m_sharedBoxZoom = m_sharedModeBar->addAction(tr("Box zoom"));
    m_sharedTrack = m_sharedModeBar->addAction(tr("Track"));
    m_sharedReset = m_sharedModeBar->addAction(tr("Reset"));
    m_sharedExpand = m_sharedModeBar->addAction(tr("Expand"));
    for (QAction* a : {m_sharedPan, m_sharedBoxZoom, m_sharedTrack})
    {
        if (a)
            a->setCheckable(true);
    }
    if (m_sharedExpand)
        m_sharedExpand->setCheckable(true);

    connect(m_sharedPan, &QAction::triggered, this, [this]() {
        if (UWatchChart* c = getChart(m_activeChartIndex))
            c->triggerModePan();
        syncSharedModeBarFromActive();
    });
    connect(m_sharedBoxZoom, &QAction::triggered, this, [this]() {
        if (UWatchChart* c = getChart(m_activeChartIndex))
            c->triggerModeBoxZoom();
        syncSharedModeBarFromActive();
    });
    connect(m_sharedTrack, &QAction::triggered, this, [this]() {
        if (UWatchChart* c = getChart(m_activeChartIndex))
            c->triggerModeTrack();
        syncSharedModeBarFromActive();
    });
    connect(m_sharedReset, &QAction::triggered, this, [this]() {
        if (UWatchChart* c = getChart(m_activeChartIndex))
            c->triggerModeReset();
    });
    connect(m_sharedExpand, &QAction::triggered, this, [this]() {
        toggleExpandChart(m_activeChartIndex);
        syncSharedModeBarFromActive();
    });

    columnLayout->insertWidget(0, m_sharedModeBar);
    m_sharedModeBar->hide();
}

void UWatchTab::syncSharedModeBarFromActive()
{
    if (!m_sharedModeBar || !m_sharedModeBar->isVisible())
        return;
    UWatchChart* c = getChart(m_activeChartIndex);
    if (!c)
        return;
    QSignalBlocker b1(m_sharedPan);
    QSignalBlocker b2(m_sharedBoxZoom);
    QSignalBlocker b3(m_sharedTrack);
    QSignalBlocker b4(m_sharedExpand);
    if (m_sharedPan)
        m_sharedPan->setChecked(c->isModePanChecked());
    if (m_sharedBoxZoom)
        m_sharedBoxZoom->setChecked(c->isModeBoxZoomChecked());
    if (m_sharedTrack)
        m_sharedTrack->setChecked(c->isModeTrackChecked());
    if (m_sharedExpand)
    {
        m_sharedExpand->setVisible(countGraphs() > 1);
        m_sharedExpand->setChecked(isChartExpanded() && m_expandedIndex == m_activeChartIndex);
        m_sharedExpand->setText(m_sharedExpand->isChecked() ? tr("Restore") : tr("Expand"));
    }
}

void UWatchTab::setAllLegendsVisible(bool visible)
{
    for (UWatchChart* c : graph)
    {
        if (c)
            c->setLegendVisible(visible);
    }
    syncDocumentFromCharts();
    if (settingsPanel)
        settingsPanel->refreshFromTab();
}

bool UWatchTab::hidePanel(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    if (visibleSlotCount() <= 1)
        return false;
    graph[chartIndex]->setPanelVisible(false);
    graph[chartIndex]->hide();
    createGridLayout(tabRowNumber, tabColNumber);
    return true;
}

bool UWatchTab::showPanel(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    graph[chartIndex]->setPanelVisible(true);
    createGridLayout(tabRowNumber, tabColNumber);
    return true;
}

bool UWatchTab::duplicatePanel(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    const NMSDK::Plot::PlotPanel snap = graph[chartIndex]->toPlotPanel();
    createGraph();
    UWatchChart* dst = graph.last();
    dst->ensurePanelId();
    NMSDK::Plot::PlotPanel copy = snap;
    copy.id = dst->panelId();
    copy.title = snap.title.isEmpty() ? tr("Chart copy") : (snap.title + tr(" (copy)"));
    dst->applyPlotPanelMeta(copy);
    for (const NMSDK::Plot::PlotSeries& serie : snap.series)
    {
        const bool serieIsXY = serie.binding.x.kind == NMSDK::Plot::DataRoleKind::Property;
        if (serieIsXY)
        {
            dst->createSerieXY(serie.binding.channel,
                               serie.binding.x.prop.component, serie.binding.x.prop.property,
                               serie.binding.x.prop.jx, serie.binding.x.prop.jy,
                               serie.binding.y.prop.component, serie.binding.y.prop.property,
                               serie.binding.y.prop.jx, serie.binding.y.prop.jy,
                               serie.yOffset, snap.viz,
                               serie.binding.x.prop.slice, serie.binding.y.prop.slice);
        }
        else
        {
            dst->createSerie(serie.binding.channel, serie.binding.y.prop.component,
                             serie.binding.y.prop.property, QString(),
                             serie.binding.y.prop.jx < 0 ? 0 : serie.binding.y.prop.jx,
                             serie.binding.y.prop.jy < 0 ? 0 : serie.binding.y.prop.jy,
                             snap.axisXRange, serie.yOffset);
        }
    }
    // Expand grid if needed to show the new panel.
    const int need = countGraphs();
    int cols = tabColNumber > 0 ? tabColNumber : 1;
    int rows = (need + cols - 1) / cols;
    createGridLayout(rows, cols);
    setActiveChart(countGraphs() - 1);
    return true;
}

bool UWatchTab::deletePanel(int chartIndex, bool confirm)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    if (countGraphs() <= 1)
        return false;
    UWatchChart* chart = graph[chartIndex];
    const int seriesCount = chart->countSeries();
    if (confirm)
    {
        const QString msg = tr("Delete panel \"%1\" with %2 series?\nUse Undo (Ctrl+Z) to restore.")
                                .arg(chart->getChartTitle())
                                .arg(seriesCount);
        if (QMessageBox::question(this, tr("Delete panel"), msg) != QMessageBox::Yes)
            return false;
        if (m_undoStack)
        {
            m_undoStack->push(new WatchDeletePanelCommand(this, chartIndex, chart->getChartTitle()));
            return true;
        }
    }
    return deletePanelImpl(chartIndex);
}

bool UWatchTab::deletePanelImpl(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    deleteGraph(chartIndex);
    for (int i = 0; i < graph.size(); ++i)
    {
        if (graph[i])
            graph[i]->setChartIndex(i);
    }
    int cols = tabColNumber > 0 ? tabColNumber : 1;
    int rows = qMax(1, (countGraphs() + cols - 1) / cols);
    if (rows * cols < countGraphs())
        rows = (countGraphs() + cols - 1) / cols;
    createGridLayout(qMin(rows, countGraphs()), cols);
    refreshInspectorIfOpen();
    return true;
}

void UWatchTab::onDuplicatePanelRequested(int chartIndex)
{
    duplicatePanel(chartIndex);
}

void UWatchTab::onHidePanelRequested(int chartIndex)
{
    hidePanel(chartIndex);
}

void UWatchTab::onDeletePanelRequested(int chartIndex)
{
    deletePanel(chartIndex, true);
}

bool UWatchTab::quickAddTimeSeries(int chartIndex,
                                   const QString& component,
                                   const QString& property,
                                   int jx,
                                   int jy,
                                   int channel)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    UWatchChart* chart = graph[chartIndex];
    if (!chart->canAddVizKind(NMSDK::Plot::VizKind::TimeSeries))
        return false;
    chart->createSerie(channel, component, property, QString(), jx, jy,
                       chart->getAxisXrange(), 0.0);
    syncDocumentFromCharts();
    if (settingsPanel)
        settingsPanel->refreshFromTab();
    return true;
}

void UWatchTab::applyPanelBinding(UWatchChart* chart, const NMSDK::Plot::PlotPanel& panel)
{
    if (!chart)
        return;
    while (chart->countSeries() > 0)
        chart->deleteSerie(0);
    chart->applyPlotPanelMeta(panel);
    for (const NMSDK::Plot::PlotSeries& serie : panel.series)
    {
        const double time_interval = panel.axisXRange > 0 ? panel.axisXRange : chart->getAxisXrange();
        const bool serieIsXY = serie.binding.x.kind == NMSDK::Plot::DataRoleKind::Property;
        const NMSDK::Plot::VizKind wantViz = serieIsXY
            ? (NMSDK::Plot::isXYFamily(panel.viz) ? panel.viz : NMSDK::Plot::VizKind::XYLine)
            : NMSDK::Plot::VizKind::TimeSeries;
        if (!chart->canAddVizKind(wantViz))
            continue;
        if (serieIsXY)
        {
            chart->createSerieXY(
                serie.binding.channel,
                serie.binding.x.prop.component,
                serie.binding.x.prop.property,
                serie.binding.x.prop.jx,
                serie.binding.x.prop.jy,
                serie.binding.y.prop.component,
                serie.binding.y.prop.property,
                serie.binding.y.prop.jx,
                serie.binding.y.prop.jy,
                serie.yOffset,
                wantViz,
                serie.binding.x.prop.slice,
                serie.binding.y.prop.slice);
        }
        else
        {
            chart->createSerie(
                serie.binding.channel,
                serie.binding.y.prop.component,
                serie.binding.y.prop.property,
                QString(),
                serie.binding.y.prop.jx < 0 ? 0 : serie.binding.y.prop.jx,
                serie.binding.y.prop.jy < 0 ? 0 : serie.binding.y.prop.jy,
                time_interval,
                serie.yOffset);
        }
        const int idx = chart->countSeries() - 1;
        if (idx < 0)
            continue;
        if (!serie.visual.displayName.isEmpty())
            chart->setSerieName(idx, serie.visual.displayName);
        chart->setSerieWidth(idx, serie.visual.width);
        chart->setSerieLineType(idx, static_cast<Qt::PenStyle>(serie.visual.penStyle));
        chart->getSerie(idx)->setColor(serie.visual.color);
        chart->getSerie(idx)->windowSize = serie.binding.windowSize;
        chart->getSerie(idx)->xyMinIntervalMs = serie.binding.xyMinIntervalMs;
        chart->getSerie(idx)->xyMinDistance = serie.binding.xyMinDistance;
        chart->setSerieEnabled(idx, serie.enabled);
    }
}

bool UWatchTab::restorePanelSnapshot(int insertIndex, const NMSDK::Plot::PlotPanel& panel)
{
    createGraph();
    const int idx = countGraphs() - 1;
    UWatchChart* chart = getChart(idx);
    if (!chart)
        return false;
    NMSDK::Plot::PlotPanel copy = panel;
    copy.id = chart->panelId();
    applyPanelBinding(chart, copy);
    if (!panel.title.isEmpty())
        chart->setChartTitle(panel.title);
    chart->setPanelVisible(true);

    if (insertIndex >= 0 && insertIndex < graph.size() - 1)
    {
        graph.move(graph.size() - 1, insertIndex);
        for (int i = 0; i < graph.size(); ++i)
        {
            if (graph[i])
                graph[i]->setChartIndex(i);
        }
    }
    int cols = tabColNumber > 0 ? tabColNumber : 1;
    int rows = qMax(1, (countGraphs() + cols - 1) / cols);
    createGridLayout(rows, cols);
    setActiveChart(qBound(0, insertIndex, countGraphs() - 1));
    syncDocumentFromCharts();
    return true;
}

void UWatchTab::openQuickAddDialog(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.count() || !graph[chartIndex])
        return;
    UWatchQuickAddDialog dlg(application, this);
    if (dlg.exec() != QDialog::Accepted || !dlg.selectionComplete())
        return;
    quickAddTimeSeries(chartIndex,
                       dlg.componentLongName(),
                       dlg.propertyName(),
                       dlg.matrixJx(),
                       dlg.matrixJy(),
                       dlg.channelIndex());
}

void UWatchTab::refreshInspectorIfOpen()
{
    if (settingsPanel && settingsPanel->isVisible())
        settingsPanel->refreshFromTab();
}

void UWatchTab::pushSerieEnabledUndo(int chartIndex, int serieIndex, bool enabled)
{
    if (!m_undoStack)
        return;
    UWatchChart* chart = getChart(chartIndex);
    if (!chart || serieIndex < 0 || serieIndex >= chart->countSeries())
        return;
    if (chart->isSerieEnabled(serieIndex) == enabled)
        return;
    m_undoStack->push(new WatchSerieEnabledCommand(this, chartIndex, serieIndex, enabled));
}

void UWatchTab::pushSerieDeleteUndo(int chartIndex, int serieIndex)
{
    if (!m_undoStack)
        return;
    UWatchChart* chart = getChart(chartIndex);
    if (!chart || serieIndex < 0 || serieIndex >= chart->countSeries())
        return;
    m_undoStack->push(new WatchSerieDeleteCommand(this, chartIndex, serieIndex));
}

void UWatchTab::pushSerieDuplicateUndo(int chartIndex, int serieIndex)
{
    if (!m_undoStack)
        return;
    UWatchChart* chart = getChart(chartIndex);
    if (!chart || serieIndex < 0 || serieIndex >= chart->countSeries())
        return;
    m_undoStack->push(new WatchSerieDuplicateCommand(this, chartIndex, serieIndex));
}

void UWatchTab::pushSerieMoveUndo(int chartIndex, int fromIndex, int toIndex)
{
    if (!m_undoStack || fromIndex == toIndex)
        return;
    UWatchChart* chart = getChart(chartIndex);
    if (!chart || fromIndex < 0 || toIndex < 0
        || fromIndex >= chart->countSeries() || toIndex >= chart->countSeries())
        return;
    m_undoStack->push(new WatchSerieMoveCommand(this, chartIndex, fromIndex, toIndex));
}

bool UWatchTab::movePanel(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || toIndex < 0 || fromIndex >= graph.size() || toIndex >= graph.size()
        || fromIndex == toIndex)
        return false;
    graph.move(fromIndex, toIndex);
    for (int i = 0; i < graph.size(); ++i)
    {
        if (graph[i])
            graph[i]->setChartIndex(i);
    }
    createGridLayout(tabRowNumber, tabColNumber);
    setActiveChart(toIndex);
    syncDocumentFromCharts();
    refreshInspectorIfOpen();
    return true;
}

bool UWatchTab::renamePanel(int chartIndex, const QString& title)
{
    if (chartIndex < 0 || chartIndex >= graph.size() || !graph[chartIndex])
        return false;
    graph[chartIndex]->setChartTitle(title);
    syncDocumentFromCharts();
    refreshInspectorIfOpen();
    return true;
}

QString UWatchTab::watchTemplatesDir() const
{
    const QString projectDir = NMSDK::Plot::watchTemplatesRoot(application);
    if (!projectDir.isEmpty())
    {
        QDir().mkpath(projectDir);
        return projectDir;
    }
    return NMSDK::Plot::sharedWatchTemplatesRoot();
}

bool UWatchTab::saveWatchTemplateAs(const QString& filePath)
{
    syncDocumentFromCharts();
    QString err;
    if (!NMSDK::Plot::saveWatchTemplateFile(filePath, m_document, &err))
    {
        QMessageBox::warning(this, tr("Save Watch template"), err);
        return false;
    }
    return true;
}

bool UWatchTab::loadWatchTemplateFrom(const QString& filePath, bool reassignIds)
{
    NMSDK::Plot::PlotDocument doc;
    QString err;
    if (!NMSDK::Plot::loadWatchTemplateFile(filePath, doc, &err))
    {
        QMessageBox::warning(this, tr("Load Watch template"), err);
        return false;
    }
    if (reassignIds)
        NMSDK::Plot::reassignPlotObjectIds(doc);
    applyPlotDocument(doc);
    syncDocumentFromCharts();
    refreshInspectorIfOpen();
    return true;
}

void UWatchTab::saveWatchTemplateDialog()
{
    QString startDir = watchTemplatesDir();
    if (startDir.isEmpty())
        startDir = QDir::homePath();
    QDir().mkpath(startDir);
    const QString path = QFileDialog::getSaveFileName(
        this,
        tr("Save Watch template"),
        startDir + QStringLiteral("layout.watch.xml"),
        tr("Watch template (*.watch.xml);;XML (*.xml)"));
    if (path.isEmpty())
        return;
    QString out = path;
    if (!out.endsWith(QStringLiteral(".watch.xml"), Qt::CaseInsensitive)
        && !out.endsWith(QStringLiteral(".xml"), Qt::CaseInsensitive))
        out += QStringLiteral(".watch.xml");
    saveWatchTemplateAs(out);
}

void UWatchTab::loadWatchTemplateDialog()
{
    QString startDir = watchTemplatesDir();
    if (startDir.isEmpty())
        startDir = QDir::homePath();
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Load Watch template"),
        startDir,
        tr("Watch template (*.watch.xml);;XML (*.xml);;All (*)"));
    if (path.isEmpty())
        return;
    loadWatchTemplateFrom(path, true);
}

void UWatchTab::syncTimeSeriesXRangeFromActive()
{
    // Multi-panel X sync for TimeSeries: copy axisXrange + track state from active chart.
    if (m_activeChartIndex < 0 || m_activeChartIndex >= graph.size() || !graph[m_activeChartIndex])
        return;
    UWatchChart* src = graph[m_activeChartIndex];
    if (NMSDK::Plot::isXYFamily(src->getVizKind()))
        return;
    const double range = src->getAxisXrange();
    const bool track = src->getIsAxisXtrackable();
    for (int i = 0; i < graph.size(); ++i)
    {
        if (i == m_activeChartIndex || !graph[i])
            continue;
        if (NMSDK::Plot::isXYFamily(graph[i]->getVizKind()))
            continue;
        graph[i]->updateTimeIntervals(range);
        graph[i]->isAxisXtrackable = track;
    }
    syncDocumentFromCharts();
}
