#include "UWatchTab.h"
#include "ui_UWatchTab.h"
#include "UGuiTelemetry.h"
#include "Plot/PlotDataAdapter.h"
#include "Plot/PlotSettingsSidePanel.h"
#include "Plot/PlotSurface.h"
#include "../../Core/Serialize/USerStorageXML.h"

#include <QHBoxLayout>



UWatchTab::UWatchTab(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UWatchTab)
{
    ui->setupUi(this);
    colSplitter = nullptr;

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    chartsHost = new QWidget(mainSplitter);
    auto* chartsLayout = new QHBoxLayout(chartsHost);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    chartsLayout->setSpacing(0);
    // Move existing horizontalLayout content hosting into chartsHost via reparent:
    // createSplitterGrid adds colSplitter into chartsHost layout.
    ui->horizontalLayout->addWidget(mainSplitter);

    createGridLayout(1,1);
    ensureSettingsPanel();
    syncDocumentFromCharts();
    setActiveChart(0);

    UpdateInterval = UpdateIntervalMs;
    setAccessibleName("UWatchTab");
}

UWatchTab::~UWatchTab()
{
    delete ui;
}

void UWatchTab::createGraph()
{
    graph.push_back(new NMSDK::Plot::PlotSurface(this));
    graph.last()->setChartIndex(graph.count()-1);

    connect(graph.last(), SIGNAL(addSerieSignal(int)), this, SLOT(createSelectionDialogSlot(int)));
    connect(graph.last(), SIGNAL(openSettingsPanel(int,bool)), this, SLOT(openSettingsPanelSlot(int,bool)));
    connect(graph.last(), SIGNAL(chartActivated(int)), this, SLOT(onChartActivated(int)));
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
        const NMSDK::Plot::VizKind panelViz = graph[graphIndex]->getVizKind();
        const bool panelIsXY = panelViz == NMSDK::Plot::VizKind::XYLine
                               || panelViz == NMSDK::Plot::VizKind::XYScatter;

        int i = 0;
        while (i < graph[graphIndex]->countSeries())
        {
            UWatchSerie* serie = graph[graphIndex]->getSerie(i);
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
                graph[graphIndex]->deleteSerie(i);
            } else {
                ++i;
            }
        }

        for (int serieIndex = 0; serieIndex < graph[graphIndex]->countSeries(); serieIndex++)
        {
            UWatchSerie *current_serie = graph[graphIndex]->getSerie(serieIndex);
            if (!current_serie || !current_serie->isOnline) {
                if (current_serie)
                {
                    RDK::UControllerDataReader* probe = env->GetDataReader(
                        current_serie->nameComponent.toStdString(),
                        current_serie->nameProperty.toStdString(),
                        current_serie->Jx < 0 ? 0 : current_serie->Jx,
                        current_serie->Jy < 0 ? 0 : current_serie->Jy);
                    current_serie->setOnlineStatus(probe != nullptr);
                }
                continue;
            }

            NMSDK::Plot::PlotSeries dto = current_serie->toPlotSeries();
            QVector<QPointF> samplePoints;
            const bool isXY = current_serie->vizKind == NMSDK::Plot::VizKind::XYLine
                              || current_serie->vizKind == NMSDK::Plot::VizKind::XYScatter
                              || panelIsXY;

            if (isXY && dto.binding.x.kind == NMSDK::Plot::DataRoleKind::Property)
            {
                samplePoints = NMSDK::Plot::samplePropertyPair(
                    env.Get(),
                    dto,
                    current_serie->YShift,
                    current_serie->xyRing,
                    current_serie->xyLastXCount,
                    current_serie->xyLastYCount);
            }
            else
            {
                samplePoints = NMSDK::Plot::sampleTimeSeries(
                    env.Get(), dto, current_serie->YShift);
            }

            RDK::UControllerDataReader* data_reader = env->GetDataReader(
                current_serie->nameComponent.toStdString(),
                current_serie->nameProperty.toStdString(),
                current_serie->Jx < 0 ? 0 : current_serie->Jx,
                current_serie->Jy < 0 ? 0 : current_serie->Jy);
            current_serie->setOnlineStatus(data_reader != nullptr);

            if (!samplePoints.isEmpty())
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
                    {
                        QVector<QPointF> decimated;
                        const int step = (newCount + decimationThreshold - 1) / decimationThreshold;
                        decimated.reserve(newCount / step + 1);
                        for (int p = 0; p < newCount; p += step)
                            decimated.push_back(samplePoints.at(p));
                        if (decimated.last() != samplePoints.last())
                            decimated.push_back(samplePoints.last());
                        current_serie->replace(decimated);
                    }
                    else
                    {
                        current_serie->replace(samplePoints);
                    }
                }

                if (!isXY)
                {
                    if (x_min == 0.0 || x_min > samplePoints.first().x())
                        x_min = samplePoints.first().x();
                    if (x_max < samplePoints.last().x())
                        x_max = samplePoints.last().x();
                }
                else
                {
                    for (const QPointF& pt : samplePoints)
                    {
                        if (x_min == 0.0 || x_min > pt.x())
                            x_min = pt.x();
                        if (x_max < pt.x())
                            x_max = pt.x();
                    }
                }
            }
        }

        if (!graph[graphIndex]->checkZoomed())
        {
            if (!panelIsXY)
            {
                if (x_max - x_min < graph[graphIndex]->getAxisXrange()) {
                    x_max = x_min + graph[graphIndex]->getAxisXrange();
                }
            }
            if (graph[graphIndex]->getIsAxisXtrackable() || panelIsXY)
            {
                graph[graphIndex]->setAxisXmax(x_max);
                graph[graphIndex]->setAxisXmin(x_min);
                graph[graphIndex]->fixInitialAxesState();
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
}

void UWatchTab::onChartActivated(int chartIndex)
{
    setActiveChart(chartIndex);
}

void UWatchTab::onInspectorActiveChartChanged(int chartIndex)
{
    setActiveChart(chartIndex);
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
        mainSplitter->setSizes({total - 300, 300});
    }
    else
    {
        settingsPanel->hide();
        mainSplitter->setSizes({1, 0});
    }
}

void UWatchTab::layoutOptionTriggered()
{
    showInspector(PlotInspectorPage::Layout, m_activeChartIndex);
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
    connect(settingsPanel, &PlotSettingsSidePanel::activeChartChanged,
            this, &UWatchTab::onInspectorActiveChartChanged);
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

void UWatchTab::deleteGraphs(int new_graph_count)
{
    int graphs_to_remove = countGraphs() - new_graph_count;

    if(graphs_to_remove < 0)
    {
        graphs_to_remove = 0;
    }

    for(int i=0; i < graphs_to_remove; i++)
        delete graph.takeLast();

    for (int i = tabRowNumber-1; i >= 0; --i)
    {
        int widget_count = rowSplitter[i]->count();
        for(int j = 0; j < widget_count; j++)
            rowSplitter[i]->widget(0)->setParent(nullptr);

        delete rowSplitter.takeLast();
    }

    if (colSplitter !=nullptr)
    {
        if (chartsHost && chartsHost->layout())
            chartsHost->layout()->removeWidget(colSplitter);
        else
            ui->horizontalLayout->removeWidget(colSplitter);
        delete colSplitter;
        colSplitter = nullptr;
    }
}



void UWatchTab::createGridLayout(int rowNumber, int colNumber)
{
    // Очистка лишних графиков (в функцию передается новое кол-во графиков)
    deleteGraphs(rowNumber*colNumber);

    tabColNumber=colNumber;
    tabRowNumber=rowNumber;

    createSplitterGrid(rowNumber);


    // create graphs if needed
    while(countGraphs() < tabColNumber*tabRowNumber)
    {
        createGraph();
    }
    int width = this->width();
    int k = 0;
    for(int i=0; i < rowNumber;i++)
    {
        QList<int> sizes;
        for(int j=0; j < colNumber; j++)
        {
            //createGraph();
            graph[k]->setChartTitle(QString("Grid graph %1").arg(QString::number((i)*colNumber+j+1)));

            rowSplitter[i]->addWidget(graph[k]);
            sizes.push_back(width/colNumber);
            k++;
        }
        rowSplitter[i]->setSizes(sizes);
    }
    setActiveChart(m_activeChartIndex);
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

    int channelIndex = 0;
    QString componentName;
    QString componentProperty;

    UComponentPropertySelectionWidget dialog(this, 3, application, 1);
    dialog.setModal(true);
    if (dialog.exec())
    {
         channelIndex = dialog.componentsList->getSelectedChannelIndex();
         componentName = dialog.componentsList->getSelectedComponentLongName();
         componentProperty = dialog.componentsList->getSelectedPropertyName();
    }

    if(!componentName.isEmpty() && !componentProperty.isEmpty())
    {
        bool is_int_or_double = false;

        {
            RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();

            RDK::UContainer *cont = model->GetComponentL(componentName.toStdString());
            if(!cont)
                return;

            RDK::UEPtr<RDK::UIProperty> prop=cont->FindProperty(componentProperty.toStdString());
            if(!prop)
                return;

            if(prop->GetLanguageType() == typeid(double) || prop->GetLanguageType() == typeid(int))
            {
                is_int_or_double = true;
            }
        }

        const double time_interval = graph[chartIndex]->getAxisXrange();
        if (graph[chartIndex]->getVizKind() == NMSDK::Plot::VizKind::XYLine
            || graph[chartIndex]->getVizKind() == NMSDK::Plot::VizKind::XYScatter)
        {
            // First dialog picks Y; second picks X.
            QString xComponent;
            QString xProperty;
            int xJx = 0;
            int xJy = 0;
            UComponentPropertySelectionWidget xDialog(this, 3, application, 1);
            xDialog.setWindowTitle(tr("Select X source"));
            xDialog.setModal(true);
            if (!xDialog.exec())
                return;
            xComponent = xDialog.componentsList->getSelectedComponentLongName();
            xProperty = xDialog.componentsList->getSelectedPropertyName();
            if (xComponent.isEmpty() || xProperty.isEmpty())
                return;

            int yJx = 0;
            int yJy = 0;
            if (!is_int_or_double)
            {
                UMatrixFormDialog* form = new UMatrixFormDialog();
                form->SelectMatrix(componentName.toStdString(), componentProperty.toStdString());
                if (form->exec() == QDialog::Accepted && !form->SelectedRows.empty())
                {
                    yJx = form->SelectedRows[0];
                    yJy = form->SelectedCols.empty() ? 0 : form->SelectedCols[0];
                }
                delete form;
            }
            {
                bool xScalar = true;
                RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>();
                RDK::UContainer* cont = model->GetComponentL(xComponent.toStdString());
                if (cont)
                {
                    RDK::UEPtr<RDK::UIProperty> prop = cont->FindProperty(xProperty.toStdString());
                    if (prop
                        && prop->GetLanguageType() != typeid(double)
                        && prop->GetLanguageType() != typeid(int))
                    {
                        xScalar = false;
                    }
                }
                if (!xScalar)
                {
                    UMatrixFormDialog* form = new UMatrixFormDialog();
                    form->SelectMatrix(xComponent.toStdString(), xProperty.toStdString());
                    if (form->exec() == QDialog::Accepted && !form->SelectedRows.empty())
                    {
                        xJx = form->SelectedRows[0];
                        xJy = form->SelectedCols.empty() ? 0 : form->SelectedCols[0];
                    }
                    delete form;
                }
            }

            graph[chartIndex]->createSerieXY(
                channelIndex, xComponent, xProperty, xJx, xJy, componentName, componentProperty,
                yJx, yJy, 0.0, graph[chartIndex]->getVizKind());
            return;
        }

        if(is_int_or_double)
        {
            graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, QString(), 0, 0, time_interval, 0.0);
            return;
        }

        UMatrixFormDialog* form = new UMatrixFormDialog();
        form->SelectMatrix(componentName.toStdString(),componentProperty.toStdString());

        if(form->exec()== QDialog::Accepted)
        {
            if(form->SelectedRows.empty() || form->SelectedCols.empty())
             {
                form->SelectedRows = {0};
                form->SelectedCols = {0};
            }

            for(int i = 0; i < form->SelectedRows.size(); i++)
                graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, QString(), form->SelectedRows[i], form->SelectedCols[i], time_interval, 0.0);
        }
        delete form;
    }
    syncDocumentFromCharts();
}

void UWatchTab::createXYSelectionDialog(int chartIndex)
{
    if (chartIndex < 0 || chartIndex >= graph.count() || !graph[chartIndex])
        return;
    graph[chartIndex]->setVizKind(NMSDK::Plot::VizKind::XYLine);
    createSelectionDialog(chartIndex);
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
    xml.DelNodeInternalContent();
    syncDocumentFromCharts();
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
    createGridLayout(doc.gridRows, doc.gridCols);
    if (doc.panels.size() != countGraphs())
        return;

    for (int graphIndex = 0; graphIndex < doc.panels.size(); ++graphIndex)
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
            if (serie.binding.x.kind == NMSDK::Plot::DataRoleKind::Property)
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
                    panel.viz == NMSDK::Plot::VizKind::TimeSeries ? NMSDK::Plot::VizKind::XYLine
                                                                  : panel.viz);
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
        }
    }
    applySplitterSizes(doc);
    m_document = doc;
    m_document.schemaVersion = NMSDK::Plot::PlotDocument::CurrentSchemaVersion;
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
